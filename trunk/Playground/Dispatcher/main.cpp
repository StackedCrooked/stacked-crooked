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
#include <functional>
#include <iostream>


//using Poco::Net::ServerSocket;
//using Poco::Net::HTTPRequestHandler;
//using Poco::Net::HTTPRequestHandlerFactory;
//using Poco::Net::HTTPServerRequest;
//using Poco::Net::HTTPServerResponse;
//using Poco::Net::HTTPServerParams;
//using Poco::Timestamp;
//using Poco::DateTimeFormatter;
//using Poco::DateTimeFormat;
//using Poco::ThreadPool;
//using Poco::Util::ServerApplication;
//using Poco::Util::Application;
//using Poco::Util::Option;
//using Poco::Util::OptionSet;
//using Poco::Util::HelpFormatter;


class HTTPServer: public Poco::Util::ServerApplication
{
public:
    typedef std::function<void(Poco::Net::HTTPServerRequest&, Poco::Net::HTTPServerResponse&)> HandleRequest;

    HTTPServer(const HandleRequest & inHandleRequest) : mHandleRequest(inHandleRequest)
    {
    }

private:
    int main(const std::vector<std::string>& )
    {
        Poco::ThreadPool::defaultPool().addCapacity(16);
        Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
        pParams->setMaxQueued(100);
        pParams->setMaxThreads(Poco::ThreadPool::defaultPool().capacity());

        // set-up a server socket
        Poco::Net::ServerSocket svs(8080);


        struct RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
        {
            RequestHandlerFactory(const HandleRequest & inHandleRequest) : mHandleRequest(inHandleRequest) {}

            Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest&)
            {
                struct RequestHandler: public Poco::Net::HTTPRequestHandler
                {
                    RequestHandler(const HandleRequest & inHandleRequest) : mHandleRequest(inHandleRequest) {}

                    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
                    {
                        mHandleRequest(request, response);
                    }

                    HandleRequest mHandleRequest;
                };
                return new RequestHandler(mHandleRequest);
            }

            HandleRequest mHandleRequest;
        };

        // set-up a HTTPServer instance
        Poco::Net::HTTPServer srv(new RequestHandlerFactory(mHandleRequest), svs, pParams);

        // start the HTTPServer
        srv.start();

        // wait for CTRL-C or kill
        waitForTerminationRequest();

        // Stop the HTTPServer
        srv.stop();

        return Application::EXIT_OK;
    }

    HandleRequest mHandleRequest;
};


struct Dispatcher : HTTPServer
{
    Dispatcher() :
        HTTPServer(std::bind(&Dispatcher::handleRequest,
                             this,
                             std::placeholders::_1,
                             std::placeholders::_2))
    {
    }

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        Poco::StreamCopier::copyStream(request.stream(), response.send());
    }
};



int main(int argc, char** argv)
{
    Dispatcher app;
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
