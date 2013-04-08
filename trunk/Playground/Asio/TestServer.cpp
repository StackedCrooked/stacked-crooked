#include "MessageServer.h"


int main(int argc, char * argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: chat_server <port> [<port> ...]\n";
            return 1;
        }

        boost::asio::io_service io_service;

        Servers servers;
        for (int i = 1; i < argc; ++i)
        {
            using namespace std; // For atoi.
            boost::asio::ip::tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
            ServerPtr server(new MessageServer(io_service, endpoint));
            servers.push_back(server);
        }

        io_service.run();
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
