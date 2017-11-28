//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>


class EchoSession
{
public:
    EchoSession(boost::asio::io_service& io_service) :
        mSocket(io_service)
    {
    }

    boost::asio::ip::tcp::socket& socket()
    {
        return mSocket;
    }

    void start()
    {
        mSocket.async_read_some(
            boost::asio::buffer(data_, max_length),
            boost::bind(
                &EchoSession::handle_read,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

private:
    void handle_read(boost::system::error_code error, size_t bytes_transferred)
    {
        if (!error)
        {
            boost::asio::async_write(
                mSocket,
                boost::asio::buffer(data_, bytes_transferred),
                boost::bind(&EchoSession::handle_write, this, boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
    }

    void handle_write(boost::system::error_code error)
    {
        if (!error)
        {
            mSocket.async_read_some(
                boost::asio::buffer(data_, max_length),
                boost::bind(
                    &EchoSession::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

    boost::asio::ip::tcp::socket mSocket;
    enum { max_length = 1024 };
    char data_[max_length];
};


class EchoServer
{
public:
    EchoServer(boost::asio::io_service& io_service, short port)
        : io_service_(io_service),
          acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        EchoSession* new_session = new EchoSession(io_service_);

        acceptor_.async_accept(
            new_session->socket(),
            boost::bind(
                &EchoServer::handle_accept,
                this,
                new_session,
                boost::asio::placeholders::error));
    }

    void handle_accept(EchoSession* new_session, boost::system::error_code error)
    {
        if (!error)
        {
            new_session->start();
        }
        else
        {
            delete new_session;
        }

        start_accept();
    }

    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        using namespace std; // For atoi.
        EchoServer s(io_service, atoi(argv[1]));

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
