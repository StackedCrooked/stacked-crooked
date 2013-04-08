#include "Message.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <deque>
#include <future>
#include <iostream>
#include <stdint.h>


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
    
    std::future<std::string> write(const std::string & msg)
    {
        Message message(msg);
        io_service_.post(boost::bind(&MessageClient::do_write, this, msg));
        mPromise.reset(new std::promise<std::string>);
        return mPromise->get_future();
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
        try
        {
            if (error)
            {
                throw boost::system::system_error(error);
            }
            
            if (!mReadMessage.decode_header())
            {
                throw std::runtime_error("Failed to decode message header.");
            }
            
            boost::asio::async_read(mSocket,
                                    boost::asio::buffer(mReadMessage.body(), mReadMessage.body_length()),
                                    boost::bind(&MessageClient::handle_read_body, this, boost::asio::placeholders::error));
        }
        catch (...)
        {
            mPromise->set_exception(std::current_exception());
            do_close();
        }
    }

    void handle_read_body(const boost::system::error_code & error)
    {
        try
        {
            if (error)
            {
                throw boost::system::system_error(error);
            }
            
            mPromise->set_value(mReadMessage.body());
            
            boost::asio::async_read(mSocket,
                                    boost::asio::buffer(mReadMessage.data(), Message::header_length),
                                    boost::bind(&MessageClient::handle_read_header, this,
                                                boost::asio::placeholders::error));
        }
        catch (...)
        {
            mPromise->set_exception(std::current_exception());
            do_close();
        }
    }

    void do_write(const Message & msg)
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
    std::shared_ptr<std::promise<std::string>> mPromise;
};
