//
// stream_server.cpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdio>
#include <iostream>
#include <deque>
#include <string>
#include <vector>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

using boost::asio::local::stream_protocol;

class Session : public boost::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::io_service& io_service)
        : mSocket(io_service)
    {
    }

    stream_protocol::socket& socket()
    {
        return mSocket;
    }

    void start()
    {
        mSocket.async_read_some(boost::asio::buffer(mData),
                                boost::bind(&Session::handle_read,
                                            shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }


    std::string generate_reply(const char* request_data, int request_size)
    {
        static const std::string open_tag = "<SSH>";
        static const std::string close_tag = "</SSH>";

        std::string result;
        result.reserve(open_tag.size() + request_size + close_tag.size());
        result += open_tag;
        result.insert(result.end(), request_data, request_data + request_size);
        result += close_tag;
        return result;
    }

    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {
        if (error)
        {
            return;
        }
            
        auto was_empty = mReplies.empty();

        mReplies.push_back(generate_reply(mData.data(), bytes_transferred));
        std::cout << "reply=[" << mReplies.back() << "]" << std::endl;

        if (was_empty)
        {
            boost::asio::async_write(
                mSocket,
                boost::asio::buffer(mReplies.front().data(), mReplies.front().size()),
                boost::bind(
                    &Session::handle_write,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (error)
        {
            return;
        }

        std::cout << "Written reply: " << mReplies.front() << std::endl;
        std::cout << "bytes_transferred=" << bytes_transferred << std::endl;

        mReplies.pop_front();

        if (!mReplies.empty())
        {
            boost::asio::async_write(
                mSocket,
                boost::asio::buffer(mReplies.front().data(), mReplies.front().size()),
                boost::bind(
                    &Session::handle_write,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            return;
        }

        mSocket.async_read_some(
            boost::asio::buffer(mData),
            boost::bind(
                &Session::handle_read,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
private:
    // The socket used to communicate with the client.
    stream_protocol::socket mSocket;

    // Buffer used to store data received from the client.
    boost::array<char, 1024> mData;
    std::deque<std::string> mReplies;
};

typedef boost::shared_ptr<Session> session_ptr;

class Server
{
public:
    Server(boost::asio::io_service& io_service, const std::string& file)
        : io_service_(io_service),
          acceptor_(io_service, stream_protocol::endpoint(file))
    {
        session_ptr new_session(new Session(io_service_));
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&Server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }

    void handle_accept(session_ptr new_session,
                       const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
        }

        new_session.reset(new Session(io_service_));
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&Server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }

private:
    boost::asio::io_service& io_service_;
    stream_protocol::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: stream_server <file>\n";
            std::cerr << "*** WARNING: existing file is removed ***\n";
            return 1;
        }

        boost::asio::io_service io_service;

        std::remove(argv[1]);
        Server s(io_service, argv[1]);

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

#else // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
# error Local sockets not available on this platform.
#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
