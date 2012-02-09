#include "Networking.h"
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>


using namespace boost::asio;
using boost::asio::ip::udp;


//
// Always returns the same io_service instance.
//
boost::asio::io_service & get_io_service()
{
    static boost::asio::io_service io;
    return io;
}


//
// UDPServer
//
struct UDPServer::Impl
{
    Impl(unsigned inPort, const RequestHandler & inRequestHandler) :
        mPort(inPort),
        mRequestHandler(inRequestHandler),
        mSocket(get_io_service(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), mPort))
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

    ~Impl()
    {
    }

    unsigned mPort;
    RequestHandler mRequestHandler;
    boost::asio::ip::udp::socket mSocket;
};


UDPServer::UDPServer(unsigned inPort, const RequestHandler & inRequestHandler) :
    mImpl(new Impl(inPort, inRequestHandler))
{
}


UDPServer::~UDPServer()
{
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


//
// UDPReceiver
//
struct UDPReceiver::Impl
{
    Impl(unsigned inPort,
         const RequestHandler & inRequestHandler,
         const StopCheck & inStopCheck) :
        mPort(inPort),
        mRequestHandler(inRequestHandler),
        mSocket(get_io_service(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), mPort)),
        mStopCheck(inStopCheck)
    {
        std::cout << "Listening to port " << mPort << std::endl;
        receive();
    }


    ~Impl()
    {
        std::cout << "Stop listening to port " << mPort << std::endl;
    }



    void receive()
    {
        if (!mStopCheck())
        {
            std::cout << "StopCheck returns false. Waiting to receive data" << std::endl;
            mSocket.async_receive_from(boost::asio::buffer(mData, cMaxLength), mSenderEndpoint,
                                       boost::bind(&Impl::onReceive, this,
                                                   boost::asio::placeholders::error,
                                                   boost::asio::placeholders::bytes_transferred));

        }
        else
        {
            std::cout << "StopCheck returns true. Stop trying to receive." << std::endl;
        }
    }

    void onReceive(const boost::system::error_code & inError,
                 std::size_t inSize)
    {
        if (!inError && inSize > 0)
        {
            std::cout << "Received data: " << std::string(mData, inSize) << std::endl;
            mRequestHandler(std::string(mData, inSize));
            receive();
        }
        else
        {
            std::cout << "BAD: size: " << inSize << ". error: " << inError.message() << std::endl;
            receive();
        }
    }

    unsigned mPort;
    RequestHandler mRequestHandler;
    boost::asio::ip::udp::socket mSocket;
    udp::endpoint mSenderEndpoint;
    StopCheck mStopCheck;
    enum { cMaxLength = 1024 * 1024 };
    char mData[cMaxLength];
};


UDPReceiver::UDPReceiver(unsigned inPort,
                         const RequestHandler & inRequestHandler,
                         const StopCheck & inStopCheck) :
    mImpl(new Impl(inPort, inRequestHandler, inStopCheck))
{
}


UDPReceiver::~UDPReceiver()
{
}



//
// UDPSender
//
struct UDPSender::Impl : boost::noncopyable
{
    Impl(const std::string & inURL, unsigned inPort) :
        socket(get_io_service(), udp::endpoint(udp::v4(), 0)),
        resolver(get_io_service()),
        query(udp::v4(), inURL.c_str(), boost::lexical_cast<std::string>(inPort).c_str()),
        iterator(resolver.resolve(query))
    {
        std::cout << "UDPSender. Dest: " << inURL << " Port: " << inPort << std::endl;
    }

    ~Impl()
    {
        std::cout << "~UDPSender" << std::endl;
    }

    udp::socket socket;
    udp::resolver resolver;
    udp::resolver::query query;
    udp::resolver::iterator iterator;
};


UDPSender::UDPSender(const std::string & inURL, unsigned inPort) :
    mImpl(new Impl(inURL, inPort))
{
}


UDPSender::~UDPSender()
{
}


void UDPSender::send(const std::string & inMessage)
{
    std::cout << "Sending: " << inMessage << std::endl;
    mImpl->socket.send_to(boost::asio::buffer(inMessage.c_str(), inMessage.size()), *mImpl->iterator);
}
