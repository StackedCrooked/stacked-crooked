#if 0
#include "TCPClient.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>


boost::asio::io_service & get_io_service();


using boost::asio::ip::tcp;


struct TCPClient::Impl
{
    Impl(const std::string & inURL, short inPort) :
        mURL(inURL),
        mPort(inPort),
        resolver(get_io_service()),
        query(tcp::v4(), inURL, boost::lexical_cast<std::string>(inPort)),
        iterator(resolver.resolve(query)),
        socket(get_io_service())
    {
        boost::asio::connect(socket, iterator);
    }

    std::string send(const std::string & inRequest)
    {
        boost::asio::write(socket, boost::asio::buffer(inRequest.data(), inRequest.size()));

        char data[1024 * 1024];
        std::size_t length = boost::asio::read(socket, boost::asio::buffer(data, sizeof(data)));
        return std::string(data, length);
    }


    std::string mURL;
    short mPort;

    tcp::resolver resolver;
    tcp::resolver::query query;
    tcp::resolver::iterator iterator;
    tcp::socket socket;

};


TCPClient::TCPClient(const std::string & inURL, short inPort) :
    mImpl(new Impl(inURL, inPort))
{
}


TCPClient::~TCPClient()
{
}


std::string TCPClient::send(const std::string & inRequest)
{
    return mImpl->send(inRequest);
}
#endif
