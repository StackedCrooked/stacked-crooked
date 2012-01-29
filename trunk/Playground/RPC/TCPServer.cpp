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
    Session() :
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
        if (!error)
        {
            boost::asio::async_write(mSocket,
                                     boost::asio::buffer(mData, bytes_transferred),
                                     boost::bind(&Session::handle_write, this,
                                                 boost::asio::placeholders::error));
        }
    }

    void handle_write(const boost::system::error_code & error)
    {
        if (!error)
        {
            mSocket.async_read_some(boost::asio::buffer(mData, cLimit),
                                    boost::bind(&Session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
    }

private:
    tcp::socket mSocket;
    enum { cLimit = 1024 };
    char mData[cLimit];
};


typedef boost::shared_ptr<Session> SessionPtr;


} // anonymous namespace


struct TCPServer::Impl
{
    Impl(short inPort) :
        mAcceptor(get_io_service(), tcp::endpoint(tcp::v4(), inPort))
    {
        SessionPtr session(new Session());
        mAcceptor.async_accept(session->socket(),
                               boost::bind(&Impl::handle_accept, this, session,
                                           boost::asio::placeholders::error));
    }

    ~Impl()
    {

    }

    void handle_accept(SessionPtr new_session,
                       const boost::system::error_code & error)
    {
        if (!error)
        {
            new_session->start();
            new_session.reset(new Session());
            mAcceptor.async_accept(new_session->socket(),
                                   boost::bind(&Impl::handle_accept, this, new_session,
                                               boost::asio::placeholders::error));
        }
    }

private:
    tcp::acceptor mAcceptor;
};


TCPServer::TCPServer(short inPort) :
    mImpl(new Impl(inPort))
{
}


TCPServer::~TCPServer()
{
}
