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
#include <iostream>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <thread>


using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;


struct PublicRequestHandler : public HTTPRequestHandler
{
    PublicRequestHandler() :
        mMutex(),
        mCondition(),
        mReq(),
        mQuit()
    {
    }

    ~PublicRequestHandler()
    {
        assert(mQuit);
    }

    void setQuitFlag()
    {
        mQuit = true;
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        while (!mQuit)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            mCondition.wait(lock);
            if (mQuit)
            {
                return;
            }
            Poco::StreamCopier::copyStream(mReq->stream(), response.send());
            mReq = nullptr;
        }
    }

    void push(HTTPServerRequest & req)
    {
        mReq = &req;
        mCondition.notify_one();
    }

    mutable std::mutex mMutex;
    std::condition_variable mCondition;
    HTTPServerRequest * mReq;
    std::atomic<bool> mQuit;
};


struct TimeRequestHandlerFactory: public HTTPRequestHandlerFactory
{
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
    {
        if (request.getURI() == "/")
            return new PublicRequestHandler;
        else
            return 0;
    }
};


class HTTPTimeServer: public Poco::Util::ServerApplication
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
    HTTPTimeServer(): _helpRequested(false)
    {
    }

    ~HTTPTimeServer()
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

    int main(const std::vector<std::string>& args)
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
            HTTPServer srv(new TimeRequestHandlerFactory, svs, pParams);
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
    HTTPTimeServer app;
    return app.run(argc, argv);
}
