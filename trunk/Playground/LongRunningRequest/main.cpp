#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/StreamCopier.h"
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>


using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;


struct ServiceImplementor : public HTTPRequestHandler
{
    ServiceImplementor() :
        mNewJobMutex(),
        mNewJobCondition(),
        mNewJob()
    {
    }

    enum class Type {
        JobRequest,
        JobResult
    };

    Type parseType(const std::string & str) const
    {
        if (str == "JobRequest")
        {
            return Type::JobRequest;
        }
        else
        {
            return Type::JobResult;
        }
    }

    std::pair<Type, std::string> getReq(HTTPServerRequest & httpReq)
    {
        std::string type;
        httpReq.stream() >> type;

        std::string body;
        httpReq.stream() >> body;
        return std::make_pair(parseType(type), body);
    }

    void handleRequest(HTTPServerRequest& httpReq, HTTPServerResponse& resp)
    {
        auto req = getReq(httpReq);
        if (req.first == Type::JobRequest)
        {
            // Wait for the user to submit a job.
            std::unique_lock<std::mutex> lock(mNewJobMutex);
            mNewJobCondition.wait(lock);
            resp.send() << mNewJob;
        }
        else if (req.first == Type::JobResult)
        {
            std::unique_lock<std::mutex> lock(mJobResultMutex);
            mJobResult = req.second;
            mJobResultCondition.notify_one();
        }
    }

    std::future<std::string> submitJob(const std::string & inJob)
    {
        std::unique_lock<std::mutex> lock(mNewJobMutex);
        mNewJob = inJob;
        mNewJobCondition.notify_one();

        Task task([this]() -> std::string {
            std::unique_lock<std::mutex> resultlock(mJobResultMutex);
            mJobResultCondition.wait(resultlock);
            return mJobResult;
        });
        auto result = task.get_future();
        mTasks.insert(std::make_pair(inJob, std::make_shared<decltype(task)>(std::move(task))));
        return result;
    }

    mutable std::mutex mNewJobMutex;
    std::condition_variable mNewJobCondition;
    std::string mNewJob;

    mutable std::mutex mJobResultMutex;
    std::condition_variable mJobResultCondition;
    std::string mJobResult;

    typedef std::packaged_task<std::string()> Task;
    typedef std::shared_ptr<Task> TaskPtr;
    typedef std::map<std::string, TaskPtr > Tasks;
    Tasks mTasks;
};


struct ServiceProvider : public HTTPRequestHandler
{
    ServiceProvider(ServiceImplementor & inImplementor) :
        mImplementor(inImplementor)
    {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        std::string job;
        request.stream() >> job;
        std::future<std::string> job_result = mImplementor.submitJob(job);
        response.send() << job_result.get();
    }

    ServiceImplementor & mImplementor;
};



struct RequestHandlerFactory: public HTTPRequestHandlerFactory, ServiceImplementor
{
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
    {
        std::cout << request.getURI() << std::endl;
        if (request.getURI() == "/ServiceImplementor")
        {
            return new ServiceImplementor();
        }
        else if (request.getURI() == "/ServiceProvider")
        {
            return new ServiceProvider(*this);
        }
        else
        {
            throw std::runtime_error(request.getURI());
        }
    }
};


class ColiruServer : public Poco::Util::ServerApplication
    /// The main application class.
    ///
    /// This class handles command-line arguments and
    /// configuration files.
    /// Start the HTTPTimeServer executable with the help
    /// option (/help on Windows, --help on Unix) for
    /// the available command line options.
    ///
    /// To use the sample configuration file (HTTPTimeServer.properties),
    /// copy the file to the directory where the HTTPTimeServer executable
    /// resides. If you start the debug version of the HTTPTimeServer
    /// (HTTPTimeServerd[.exe]), you must also create a copy of the configuration
    /// file named HTTPTimeServerd.properties. In the configuration file, you
    /// can specify the port on which the server is listening (default
    /// 9980) and the format of the date/time string sent back to the client.
    ///
    /// To test the TimeServer you can use any web browser (http://localhost:9980/).
{
public:
    ColiruServer(): _helpRequested(false)
    {
    }

    ~ColiruServer()
    {
    }

protected:
    void initialize(Application& self)
    {
        loadConfiguration(); // load default configuration files, if present
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet& options)
    {
        ServerApplication::defineOptions(options);

        options.addOption(
            Option("help", "h", "display help information on command line arguments")
                .required(false)
                .repeatable(false));
    }

    void handleOption(const std::string& name, const std::string& value)
    {
        ServerApplication::handleOption(name, value);

        if (name == "help")
            _helpRequested = true;
    }

    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A web server that serves the current date and time.");
        helpFormatter.format(std::cout);
    }

    int main(const std::vector<std::string>& )
    {
        if (_helpRequested)
        {
            displayHelp();
        }
        else
        {
            // get parameters from configuration file
            unsigned short port = (unsigned short) config().getInt("HTTPTimeServer.port", 9980);
            std::string format(config().getString("HTTPTimeServer.format", DateTimeFormat::SORTABLE_FORMAT));
            int maxQueued  = config().getInt("HTTPTimeServer.maxQueued", 100);
            int maxThreads = config().getInt("HTTPTimeServer.maxThreads", 16);
            ThreadPool::defaultPool().addCapacity(maxThreads);

            HTTPServerParams* pParams = new HTTPServerParams;
            pParams->setMaxQueued(maxQueued);
            pParams->setMaxThreads(maxThreads);

            // set-up a server socket
            ServerSocket svs(port);
            // set-up a HTTPServer instance
            HTTPServer srv(new RequestHandlerFactory, svs, pParams);
            // start the HTTPServer
            srv.start();
            // wait for CTRL-C or kill
            waitForTerminationRequest();
            // Stop the HTTPServer
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};
int main(int argc, char** argv)
{
    ColiruServer app;
    return app.run(argc, argv);
}
