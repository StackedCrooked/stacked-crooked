#include "Networking.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <chrono>
#include <iostream>


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
            char data[128 * 1024];
            udp::endpoint sender;
            auto length = mSocket.receive_from(boost::asio::buffer(data, sizeof(data)), sender);

            std::string response = inRequestHandler(std::string(data, length));
            mSocket.send_to(boost::asio::buffer(response.c_str(), response.size()), sender);
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
    Impl(const std::string& ip, unsigned port) :
        mSocket(get_io_service(), udp::endpoint(udp::v4(), 0)),
        mEndPoint(boost::asio::ip::address_v4::from_string(ip.c_str()), port)
    {
    }

    ~Impl()
    {
    }

    void send(const std::string & inMessage)
    {
        std::cout << "Send" << std::endl;
        mMessage = inMessage;
        mStartTime = std::chrono::steady_clock::now();

        send_one();
    }

    void send_one()
    {
        auto start_time = std::chrono::steady_clock::now();
        for (auto i = 0; i != 1000000; ++i)
        {
            mSocket.send_to(boost::asio::buffer(mMessage), mEndPoint);
        }
        auto elapsed_s = (std::chrono::steady_clock::now() - start_time).count() / 1e9;
        auto mpps = 1000000 / elapsed_s / 1e6;
        std::cout << "! MPPS=" << mpps << std::endl;
        mSocket.async_send_to(boost::asio::buffer(mMessage), mEndPoint, boost::bind(&Impl::on_send, this, _1, _2));
    }

    void on_send(boost::system::error_code ec, std::size_t)
    {
        if (ec)
        {
            std::cerr << "Failed to send packet: " << ec.message() << std::endl;
            return;
        }

        if (++mPacketCount == 1000000)
        {
            auto elapsed_time = std::chrono::steady_clock::now() - mStartTime;
            auto elapsed_s = elapsed_time.count() / 1e9;
            auto mpps = mPacketCount / elapsed_s / 1e6;
            std::cout << "MPPS=" << mpps << std::endl;
        }

        send_one();
    }

    udp::socket mSocket;
    boost::asio::ip::udp::endpoint mEndPoint;
    std::string mMessage;
    int64_t mPacketCount = 0;
    std::chrono::steady_clock::time_point mStartTime{};
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
    mImpl->send(inMessage);
    get_io_service().run();
    return "";
}


//
// UDPReceiver
//
struct UDPReceiver::Impl
{
    typedef UDPReceiver::RequestHandler RequestHandler;

    Impl(UDPReceiver * inUDPReceiver,
         unsigned inPort,
         const RequestHandler & inRequestHandler) :
        mUDPReceiver(inUDPReceiver),
        mPort(inPort),
        mRequestHandler(inRequestHandler),
        mSocket(get_io_service(), boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), mPort))
    {
        prepareForReceiving();
    }


    ~Impl()
    {
    }



    void prepareForReceiving()
    {
        mSocket.async_receive_from(boost::asio::buffer(mData, cMaxLength), mSenderEndpoint,
                                   boost::bind(&Impl::handleReceivedData, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred));
    }

    void handleReceivedData(const boost::system::error_code & inError, std::size_t inSize)
    {
        if (!inError && inSize > 0)
        {
            if (mRequestHandler(std::string(mData, inSize)))
            {
                prepareForReceiving();
                mUDPReceiver->waitForAll();
            }
            // else: stop listening
        }
        else
        {
            throw std::runtime_error(inError.message());
        }
    }

    UDPReceiver * mUDPReceiver;
    unsigned mPort;
    RequestHandler mRequestHandler;
    boost::asio::ip::udp::socket mSocket;
    udp::endpoint mSenderEndpoint;
    enum { cMaxLength = 1024 * 1024 };
    char mData[cMaxLength];
};


UDPReceiver::UDPReceiver(unsigned inPort, const RequestHandler & inRequestHandler) :
    mImpl(new Impl(this, inPort, inRequestHandler))
{
}


UDPReceiver::~UDPReceiver()
{
}


void UDPReceiver::waitForAll()
{
    get_io_service().run_one();
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
    }

    ~Impl()
    {
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
    mImpl->socket.send_to(boost::asio::buffer(inMessage.c_str(), inMessage.size()), *mImpl->iterator);
}
