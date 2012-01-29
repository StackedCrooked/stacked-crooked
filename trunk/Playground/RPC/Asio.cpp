#include "Asio.h"
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>


using namespace boost::asio;
using boost::asio::ip::udp;


//
// Always returns the same io_service instance.
//
static boost::asio::io_service & get_io_service()
{
    static boost::asio::io_service io;
    return io;
}


//
// UDPServer
//
struct UDPServer::Impl
{
    Impl(unsigned inPort) :
        mPort(inPort),
        mSocket(get_io_service(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), mPort))
    {
    }

    ~Impl()
    {
    }

    void run(const RequestHandler & inRequestHandler)
    {
        while (true)
        {
            const unsigned int cMaxLength = 1024 * 1024;
            char data[cMaxLength];
            udp::endpoint sender_endpoint;
            size_t length = mSocket.receive_from(boost::asio::buffer(data, cMaxLength), sender_endpoint);
            std::string response = inRequestHandler(std::string(data, length));
            mSocket.send_to(boost::asio::buffer(response.c_str(), response.size()), sender_endpoint);
        }
    }

    unsigned mPort;
    boost::asio::ip::udp::socket mSocket;
};


UDPServer::UDPServer(unsigned inPort) :
    mImpl(new Impl(inPort))
{
}


UDPServer::~UDPServer()
{
}


void UDPServer::run(const RequestHandler & inRequestHandler)
{
    mImpl->run(inRequestHandler);
}


//
// UDPClient
//
struct UDPClient::Impl : boost::noncopyable
{
    Impl(const std::string & inURL, unsigned inPort) :
        socket(get_io_service(), udp::endpoint(udp::v4(), 0)),
        resolver(get_io_service()),
        query(udp::v4(), inURL.c_str(), boost::lexical_cast<std::string>(inPort).c_str()),
        iterator(resolver.resolve(query))
    {

    }

    ~Impl()
    {
    }

    udp::socket socket;
    udp::resolver resolver;
    udp::resolver::query query;
    udp::resolver::iterator iterator;
};


UDPClient::UDPClient(const std::string & inURL, unsigned inPort) :
    mImpl(new Impl(inURL, inPort))
{
}


UDPClient::~UDPClient()
{
}


std::string UDPClient::send(const std::string & inMessage)
{
    mImpl->socket.send_to(boost::asio::buffer(inMessage.c_str(), inMessage.size()), *mImpl->iterator);

    static const unsigned cMaxLength = 1024 * 1024;
    char reply[cMaxLength];
    udp::endpoint sender_endpoint;
    size_t reply_length = mImpl->socket.receive_from(boost::asio::buffer(reply, cMaxLength), sender_endpoint);
    return std::string(reply, reply_length);

}
