#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>


using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;


class RequestHandler : public TCPServerConnection
    /// This class handles all client connections.
    ///
    /// A string with the current date and time is sent back to the client.
{
public:
    RequestHandler(const StreamSocket& s) :
        TCPServerConnection(s)
    {
    }

    void run()
    {
        Application& app = Application::instance();
        app.logger().information("Request from " + this->socket().peerAddress().toString());
    }
};


struct RequestHandlerFactory : public TCPServerConnectionFactory
{
    TCPServerConnection* createConnection(const StreamSocket& socket)
    {
        return new RequestHandler(socket);
    }
};


struct TimeServer : public Poco::Util::ServerApplication
{
    int main(const std::vector<std::string>& )
    {
        ServerSocket socket(9999);
        TCPServer server(new RequestHandlerFactory, socket);
        server.start();
        waitForTerminationRequest();
        server.stop();
        return 0;
    }
};


int main(int argc, char** argv)
{
    TimeServer app;
    return app.run(argc, argv);
}
