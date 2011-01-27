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
#include <boost/lexical_cast.hpp>
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
        static int GetPortFromArgs(const std::vector<std::string>& args, int inDefault)
        {
            for (unsigned int i = 0; i < (args.size() - 1); ++i)
            {
                if (args[i] == "-p" || args[i] == "--port")
                {
                    return boost::lexical_cast<int>(args[i+1]);
                }
            }
            return inDefault;
        }

        int main(const std::vector<std::string>& args)
        {
            Poco::Data::SQLite::Connector::registerConnector();

            int port = GetPortFromArgs(args, 80);
            std::cout << "Listening to port " << port << std::endl;

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
            mFactory->registerRequestHandler<GetHighScore_HTML>();
            mFactory->registerRequestHandler<GetHighScore_XML>();
            mFactory->registerRequestHandler<GetHighScore_Text>();

            mFactory->registerRequestHandler<GetHallOfFame_HTML>();
            mFactory->registerRequestHandler<GetHallOfFame_XML>();
            mFactory->registerRequestHandler<GetHallOfFame_Text>();

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
    int result = 0;
    try
    {
        HSServer::HighScoreServer app;
        result = app.run(argc, argv);
    }
    catch (const std::exception & exc)
    {
#ifdef _WIN32
        ::MessageBoxA(0, exc.what(), "High Score Server", MB_OK);
#else
        std::cout << exc.what() << std::endl;
#endif
    }
    return result;
}
