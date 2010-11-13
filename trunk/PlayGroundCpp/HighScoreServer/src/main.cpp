#include "RequestHandler.h"
#include "SQLRequestGenericHandler.h"
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
            
            // Ownership of the factory is passed to the HTTP server.
            Poco::Net::HTTPServer httpServer(mFactory, serverSocket, params);
            httpServer.start();
            waitForTerminationRequest();
            httpServer.stop();
            return Poco::Util::Application::EXIT_OK;
        }

    private:
        void registerRequestHandlers()
        {
            mFactory->registerRequestHandler<GetHighScoreAsHTML>();
            mFactory->registerRequestHandler<GetHighScoreAsXML>();
            mFactory->registerRequestHandler<GetHighScoreAsPlainText>();

            mFactory->registerRequestHandler<GetHallOfFameAsHTML>();
            mFactory->registerRequestHandler<GetHallOfFameAsXML>();
            mFactory->registerRequestHandler<GetHallOfFameAsPlainText>();

            mFactory->registerRequestHandler<GetHighScorePostForm>();
            mFactory->registerRequestHandler<GetHighScoreDeleteForm>();
            mFactory->registerRequestHandler<PostHightScore>();
            mFactory->registerRequestHandler<DeleteHighScore>();
        }

        RequestHandlerFactory * mFactory;
    };

} // namespace HSServer


using namespace HSServer;


int main(int argc, char** argv)
{
    HSServer::HighScoreServer app;
	return app.run(argc, argv);
}
