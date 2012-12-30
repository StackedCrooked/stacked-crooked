#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/StreamCopier.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>


using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class RequestHandler: public HTTPRequestHandler
{
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        Application& app = Application::instance();
        app.logger().information("Request from " + request.clientAddress().toString());

        Poco::StreamCopier::copyStream(request.stream(), response.send());
    }
};


class RequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest&)
    {
        return new RequestHandler;
    }
};


class HTTPDispatcher: public Poco::Util::ServerApplication
{
private:
    int main(const std::vector<std::string>& )
    {
        ThreadPool::defaultPool().addCapacity(16);

        HTTPServerParams* pParams = new HTTPServerParams;
        pParams->setMaxQueued(100);
        pParams->setMaxThreads(ThreadPool::defaultPool().capacity());

        // set-up a server socket
        ServerSocket svs(8080);
        // set-up a HTTPServer instance
        HTTPServer srv(new RequestHandlerFactory, svs, pParams);
        // start the HTTPServer
        srv.start();
        // wait for CTRL-C or kill
        waitForTerminationRequest();
        // Stop the HTTPServer
        srv.stop();
        return Application::EXIT_OK;
    }
};


int main(int argc, char** argv)
{
    HTTPDispatcher app;
    return app.run(argc, argv);
}



#if 0
struct Dispatcher
{
    Dispatcher() :
        mUserServer(9000, std::bind(&Dispatcher::handleClientRequest, this, std::placeholders::_1)),
        mJobServer(9001, std::bind(&Dispatcher::handleJobRequest, this, std::placeholders::_1)),
        mResultReceiver(9002, std::bind(&Dispatcher::receiveResult, this, std::placeholders::_1))
    {
    }

    std::string handleClientRequest(const std::string & inRequest)
    {
        {
            Lock lock(mJobMutex);
            mJob = inRequest;
            mJobCondition.notify_one();
        }

        {
            Lock lock(mResultMutex);
            mResultCondition.wait(lock);
            return mResult;
        }
    }

    std::string handleJobRequest(const std::string &)
    {
        Poco::ScopedLock<Poco::Mutex> l(mJobMutex);
        mJobCondition.wait(l);
        return mJob;
    }

    bool receiveResult(const std::string & str)
    {
        Poco::ScopedLock<Poco::Mutex> lock(mResultMutex);
        mResult = str;
        mResultCondition.notify_one();
        return false;
    }

    Poco::Net::HTTPServer mServer;


    Poco::Condition mJobCondition;
    Poco::Mutex mJobMutex;
    std::string mJob;

    Condition mResultCondition;
    Mutex mResultMutex;
    std::string mResult;
};
#endif
