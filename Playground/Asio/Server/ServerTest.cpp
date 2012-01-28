#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdlib>
#include <iostream>


using boost::asio::ip::udp;


class UDPServer
{
public:
    UDPServer(unsigned inPort) :
        mPort(inPort),
        mIOService(),
        mSocket(mIOService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), mPort))
    {
    }

    enum
    {
        cMaxLength = 1024
    };

    typedef boost::function<std::string(const std::string &)> RequestHandler;

    void run(const RequestHandler & inRequestHandler)
    {
        while (true)
        {
            char data[cMaxLength];
            udp::endpoint sender_endpoint;
            size_t length = mSocket.receive_from(boost::asio::buffer(data, cMaxLength), sender_endpoint);
            std::string response = inRequestHandler(std::string(data, length));
            mSocket.send_to(boost::asio::buffer(response.c_str(), response.size()), sender_endpoint);
        }
    }

private:
    unsigned mPort;
    boost::asio::io_service mIOService;
    boost::asio::ip::udp::socket mSocket;
};


std::string HandleRequest(const std::string & inRequest)
{
    return "Handled " + inRequest;
}


int main()
{
    UDPServer server(9001);
    server.run(boost::bind(&HandleRequest, _1));
}

//void StartServer(boost::asio::io_service & io_service, short port)
//{
//    udp::socket sock(io_service, udp::endpoint(udp::v4(), port));
//    for (;;)
//    {
//        char data[cMaxLength];
//        udp::endpoint sender_endpoint;
//        size_t length = sock.receive_from(boost::asio::buffer(data, cMaxLength), sender_endpoint);
//        std::string response = std::string(data, length); //HandleRequest(std::string(data, length));
//        sock.send_to(boost::asio::buffer(response.c_str(), response.size()), sender_endpoint);
//    }
//}

//int main(int argc, char * argv[])
//{
//    try
//    {
//        if (argc != 2)
//        {
//            std::cerr << "Usage: blocking_udp_echo_server <port>\n";
//            return 1;
//        }

//        boost::asio::io_service io_service;

//        using namespace std; // For atoi.
//        StartServer(io_service, atoi(argv[1]));
//    }
//    catch (std::exception & e)
//    {
//        std::cerr << "Exception: " << e.what() << "\n";
//    }

//    return 0;
//}
