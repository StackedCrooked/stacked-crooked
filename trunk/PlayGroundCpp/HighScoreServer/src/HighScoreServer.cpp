#include "HighScoreServer.h"
#include "HighScoreRequestHandlerFactory.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/ThreadPool.h"


namespace HSServer
{

    int HighScoreServer::main(const std::vector<std::string>& args)
    {
	    int port = config().getInt("HighScoreServer.port", 9980);
	    int maxQueued  = config().getInt("HighScoreServer.maxQueued", 100);
	    int maxThreads = config().getInt("HighScoreServer.maxThreads", 16);
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
