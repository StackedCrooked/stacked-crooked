#include "Message.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <stdint.h>


namespace MessageProtocol {


using namespace boost::asio;
using boost::bind;
typedef std::deque<Message> RequestQueue;


class MessageClient
{
public:
    MessageClient(const std::string & host, uint16_t port)
        : mIOService(get_io_service()),
          mSocket(mIOService)
    {
        
        ip::tcp::resolver resolver(mIOService);
        ip::tcp::resolver::query query(host, std::to_string(port));
        ip::tcp::resolver::iterator iterator = resolver.resolve(query);
        async_connect(mSocket,
                      iterator,
                      bind(&MessageClient::handle_connect,
                           this,
                           placeholders::error));
    }

    void write(const std::string & msg)
    {
        Message message(msg);
        mIOService.post(bind(&MessageClient::do_write, this, msg));
    }

    void close()
    {
        mIOService.post(bind(&MessageClient::do_close, this));
    }

private:
    void handle_connect(const boost::system::error_code & error)
    {
        if (!error)
        {
            async_read(mSocket,
                       buffer(mReadMessage.data(), Message::header_length),
                       bind(&MessageClient::handle_read_header, this, placeholders::error));
        }
    }

    void handle_read_header(const boost::system::error_code & error)
    {
        if (!error)
        {
            mReadMessage.decode_header();
            async_read(mSocket,
                       buffer(mReadMessage.body(), mReadMessage.body_length()),
                       bind(&MessageClient::handle_read_body, this, placeholders::error));
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
            async_read(mSocket,
                       buffer(mReadMessage.data(), Message::header_length),
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
        bool write_in_progress = !mWriteMessages.empty();
        mWriteMessages.push_back(msg);
        if (!write_in_progress)
        {
            async_write(mSocket,
                        buffer(mWriteMessages.front().data(),
                               mWriteMessages.front().length()),
                        bind(&MessageClient::handle_write, this,
                             placeholders::error));
        }
    }

    void handle_write(const boost::system::error_code & error)
    {
        if (!error)
        {
            mWriteMessages.pop_front();
            if (!mWriteMessages.empty())
            {
                async_write(mSocket,
                            buffer(mWriteMessages.front().data(), mWriteMessages.front().length()),
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
        mSocket.close();
    }

private:
    io_service & mIOService;
    ip::tcp::socket mSocket;
    Message mReadMessage;
    RequestQueue mWriteMessages;
};


} // MessageProtocol
