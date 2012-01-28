#include "Client/UDPClient.h"
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>


namespace Client {


using namespace boost::asio;
using boost::asio::ip::udp;


static boost::asio::io_service & get_io_service()
{
    static boost::asio::io_service io;
    return io;
}


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

    static const unsigned cMaxLength = 1024;
    char reply[cMaxLength];
    udp::endpoint sender_endpoint;
    size_t reply_length = mImpl->socket.receive_from(boost::asio::buffer(reply, cMaxLength), sender_endpoint);
    return std::string(reply, reply_length);

}


} // namespace Client
