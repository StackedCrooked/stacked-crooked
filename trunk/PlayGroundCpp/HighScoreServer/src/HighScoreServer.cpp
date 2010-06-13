#include "HighScoreServer.h"
#include "HighScoreRequestHandlerFactory.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/ThreadPool.h"

namespace HSServer
{

    int HighScoreServer::main(const std::vector<std::string>& args)
    {
        Poco::Data::SQLite::Connector::registerConnector();


	    int port = config().getInt("HighScoreServer.port", 9980);
	    int maxQueued  = config().getInt("HighScoreServer.maxQueued", 100);
	    
        // Only allow one thread because the database using simple locking
        int maxThreads = config().getInt("HighScoreServer.maxThreads", 1);
        Poco::ThreadPool::defaultPool().addCapacity(maxThreads);
		
        Poco::Net::HTTPServerParams * params = new Poco::Net::HTTPServerParams;
	    params->setMaxQueued(maxQueued);
	    params->setMaxThreads(maxThreads);

        Poco::Net::ServerSocket serverSocket(port);
	    Poco::Net::HTTPServer httpServer(new HighScoreRequestHandlerFactory, serverSocket, params);


	    httpServer.start();
	    waitForTerminationRequest();
	    httpServer.stop();
	    return Poco::Util::Application::EXIT_OK;
    }

} // namespace HSServer
