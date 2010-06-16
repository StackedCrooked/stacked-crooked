#include "RequestHandler.h"
#include "RequestHandlerFactory.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/ThreadPool.h"
#include <iostream>


namespace HSServer
{

    class HighScoreServer : public Poco::Util::ServerApplication
    {
    public:
        HighScoreServer() :
          mFactory(0)
        {
        }

    protected:
        int main(const std::vector<std::string>& args)
        {
            Poco::Data::SQLite::Connector::registerConnector();

            int port = config().getInt("HighScoreServer.port", 80);
            int maxQueued  = config().getInt("HighScoreServer.maxQueued", 100);
            
            // Only allow one thread because the database using simple locking
            int maxThreads = config().getInt("HighScoreServer.maxThreads", 1);
            Poco::ThreadPool::defaultPool().addCapacity(maxThreads);
            
            Poco::Net::HTTPServerParams * params = new Poco::Net::HTTPServerParams;
            params->setMaxQueued(maxQueued);
            params->setMaxThreads(maxThreads);

            Poco::Net::ServerSocket serverSocket(port);

            mFactory = new RequestHandlerFactory;
            registerRequestHandlers();
            
            Poco::Net::HTTPServer httpServer(mFactory, serverSocket, params);
            httpServer.start();
            waitForTerminationRequest();
            httpServer.stop();
            return Poco::Util::Application::EXIT_OK;
        }

    private:
        void registerRequestHandlers()
        {
            mFactory->registerRequestHandler<GetAllHighScores>();
            mFactory->registerRequestHandler<AddHighScore_GET>();
            mFactory->registerRequestHandler<AddHighScore_POST>();
            mFactory->registerRequestHandler<CommitHighScore>();
            mFactory->registerRequestHandler<CommitSucceeded>();            
        }

        RequestHandlerFactory * mFactory;
    };

} // HighScoreServer


int main(int argc, char** argv)
{
    HSServer::HighScoreServer app;
	return app.run(argc, argv);
}
