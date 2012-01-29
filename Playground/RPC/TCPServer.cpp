#include "TCPServer.h"
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;


boost::asio::io_service & get_io_service();


namespace {


class Session
{
public:
    typedef TCPServer::Handler Handler;

    Session(const Handler & inHandler) :
        mHandler(inHandler),
        mSocket(get_io_service())
    {
    }

    tcp::socket & socket()
    {
        return mSocket;
    }

    void start()
    {
        mSocket.async_read_some(boost::asio::buffer(mData, cLimit),
                                boost::bind(&Session::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void handle_read(const boost::system::error_code & error,
                     size_t bytes_transferred)
    {
        if (error)
        {
            return;
        }

        std::string response = mHandler(std::string(mData, bytes_transferred));
        boost::asio::async_write(mSocket,
                                 boost::asio::buffer(response.data(), response.size()),
                                 boost::bind(&Session::handle_write, this,
                                             boost::asio::placeholders::error));
    }

    void handle_write(const boost::system::error_code & error)
    {
        if (error)
        {
            return;
        }
        mSocket.async_read_some(boost::asio::buffer(mData, cLimit),
                                boost::bind(&Session::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

private:
    Handler mHandler;
    tcp::socket mSocket;
    enum { cLimit = 1024 };
    char mData[cLimit];
};


typedef boost::shared_ptr<Session> SessionPtr;


} // anonymous namespace


struct TCPServer::Impl
{
    typedef TCPServer::Handler Handler;

    Impl() :
        mAcceptor()
    {

    }

    void listen(short inPort, const Handler & inHandler)
    {
        mHandler = inHandler;
        mAcceptor.reset(new tcp::acceptor(get_io_service(), tcp::endpoint(tcp::v4(), inPort)));
        SessionPtr session(new Session(inHandler));
        mAcceptor->async_accept(session->socket(),
                               boost::bind(&Impl::handle_accept, this, session,
                                           boost::asio::placeholders::error));
    }

    ~Impl()
    {

    }

    void handle_accept(SessionPtr inSession,
                       const boost::system::error_code & error)
    {
        if (!error)
        {
            inSession->start();
            inSession.reset(new Session(mHandler));
            mAcceptor->async_accept(inSession->socket(),
                                   boost::bind(&Impl::handle_accept, this, inSession,
                                               boost::asio::placeholders::error));
        }
    }

private:
    boost::scoped_ptr<tcp::acceptor> mAcceptor;
    Handler mHandler;
};


TCPServer::TCPServer() :
    mImpl(new Impl())
{
}


TCPServer::~TCPServer()
{
}


void TCPServer::listen(short inPort, const Handler & inHandler)
{
    mImpl->listen(inPort, inHandler);
    get_io_service().run();
}
