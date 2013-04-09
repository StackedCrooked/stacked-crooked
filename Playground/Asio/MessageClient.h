#include "Message.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <stdint.h>


namespace MessageProtocol {



typedef std::deque<Message> RequestQueue;


class MessageClient
{
public:
    MessageClient(const std::string & host, uint16_t port)
        : io_service_(get_io_service()),
          mSocket(io_service_)
    {
        
        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
        boost::asio::async_connect(
                    mSocket,
                    iterator,
                    boost::bind(&MessageClient::handle_connect,
                                this,
                                boost::asio::placeholders::error));
    }

    void write(const std::string & msg)
    {
        Message message(msg);
        io_service_.post(boost::bind(&MessageClient::do_write, this, msg));
    }

    void close()
    {
        io_service_.post(boost::bind(&MessageClient::do_close, this));
    }

private:
    void handle_connect(const boost::system::error_code & error)
    {
        if (!error)
        {
            boost::asio::async_read(mSocket,
                                    boost::asio::buffer(mReadMessage.data(), Message::header_length),
                                    boost::bind(&MessageClient::handle_read_header, this,
                                         boost::asio::placeholders::error));
        }
    }

    void handle_read_header(const boost::system::error_code & error)
    {
        if (!error && mReadMessage.decode_header())
        {
            boost::asio::async_read(
                        mSocket,
                        boost::asio::buffer(mReadMessage.body(), mReadMessage.body_length()),
                        boost::bind(&MessageClient::handle_read_body, this, boost::asio::placeholders::error));
        }
        else
        {
            do_close();
        }
    }

    void handle_read_body(const boost::system::error_code & error)
    {
        if (!error)
        {
            std::cout.write(mReadMessage.body(), mReadMessage.body_length());
            std::cout << "\n";
            boost::asio::async_read(mSocket,
                       boost::asio::buffer(mReadMessage.data(), Message::header_length),
                       boost::bind(&MessageClient::handle_read_header, this,
                                   boost::asio::placeholders::error));
        }
        else
        {
            do_close();
        }
    }

    void do_write(Message msg)
    {
        bool write_in_progress = !mWriteMessages.empty();
        mWriteMessages.push_back(msg);
        if (!write_in_progress)
        {
            boost::asio::async_write(mSocket,
                        boost::asio::buffer(mWriteMessages.front().data(),
                               mWriteMessages.front().length()),
                        boost::bind(&MessageClient::handle_write, this,
                                    boost::asio::placeholders::error));
        }
    }

    void handle_write(const boost::system::error_code & error)
    {
        if (!error)
        {
            mWriteMessages.pop_front();
            if (!mWriteMessages.empty())
            {
                boost::asio::async_write(mSocket,
                            boost::asio::buffer(mWriteMessages.front().data(), mWriteMessages.front().length()),
                            boost::bind(&MessageClient::handle_write, this, boost::asio::placeholders::error));
            }
        }
        else
        {
            do_close();
        }
    }

    void do_close()
    {
        mSocket.close();
    }

private:
    boost::asio::io_service & io_service_;
    boost::asio::ip::tcp::socket mSocket;
    Message mReadMessage;
    RequestQueue mWriteMessages;
};


} // MessageProtocol
