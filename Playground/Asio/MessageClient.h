#include "Message.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <deque>
#include <iostream>


using namespace boost;
using namespace asio;
using ip::tcp;


typedef std::deque<Message> RequestQueue;


class MessageClient
{
public:
    MessageClient(io_service & io_service, tcp::resolver::iterator endpoint_iterator)
        : io_service_(io_service),
          socket_(io_service)
    {
        async_connect(socket_, endpoint_iterator,
                      bind(&MessageClient::handle_connect, this, placeholders::error));
    }

    void write(const Message & msg)
    {
        io_service_.post(bind(&MessageClient::do_write, this, msg));
    }

    void close()
    {
        io_service_.post(bind(&MessageClient::do_close, this));
    }

private:

    void handle_connect(const system::error_code & error)
    {
        if (!error)
        {
            async_read(socket_,
                       buffer(read_msg_.data(), Message::header_length),
                       bind(&MessageClient::handle_read_header, this,
                            placeholders::error));
        }
    }

    void handle_read_header(const system::error_code & error)
    {
        if (!error && read_msg_.decode_header())
        {
            async_read(socket_,
                       buffer(read_msg_.body(), read_msg_.body_length()),
                       bind(&MessageClient::handle_read_body, this,
                            placeholders::error));
        }
        else
        {
            do_close();
        }
    }

    void handle_read_body(const system::error_code & error)
    {
        if (!error)
        {
            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            async_read(socket_,
                       buffer(read_msg_.data(), Message::header_length),
                       bind(&MessageClient::handle_read_header, this,
                            placeholders::error));
        }
        else
        {
            do_close();
        }
    }

    void do_write(Message msg)
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            async_write(socket_,
                        buffer(write_msgs_.front().data(),
                               write_msgs_.front().length()),
                        bind(&MessageClient::handle_write, this,
                             placeholders::error));
        }
    }

    void handle_write(const system::error_code & error)
    {
        if (!error)
        {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
                async_write(socket_,
                            buffer(write_msgs_.front().data(), write_msgs_.front().length()),
                            bind(&MessageClient::handle_write, this, placeholders::error));
            }
        }
        else
        {
            do_close();
        }
    }

    void do_close()
    {
        socket_.close();
    }

private:
    io_service & io_service_;
    tcp::socket socket_;
    Message read_msg_;
    RequestQueue write_msgs_;
};
