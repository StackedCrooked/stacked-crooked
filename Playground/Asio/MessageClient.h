#include "Message.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <map>
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
          mSocket(mIOService),
          mReadMessage("")
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

    void send(const std::string & request, const ClientCallback & inCallback)
    {
        Message message(request);
        auto id = message.get_id();
        mCallbacks.insert(std::make_pair(id, inCallback));
        mIOService.post(bind(&MessageClient::do_write, this, message));
        std::cout << "Sent message with id " << id << std::endl;
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
                       buffer(mReadMessage.header(), mReadMessage.header_length()),
                       bind(&MessageClient::handle_read_header, this, placeholders::error));
        }
    }

    void handle_read_header(const boost::system::error_code & error)
    {
        if (!error)
        {
            std::cout << "Response id before decoding: " << mReadMessage.get_id() << std::endl;
            mReadMessage.decode_header();
            std::cout << "Response id after decoding: " << mReadMessage.get_id() << std::endl;
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
            auto id = mReadMessage.get_id();
            auto it = mCallbacks.find(id);
            if (it != mCallbacks.end())
            {
                try
                {
                    auto callback = it->second;
                    mCallbacks.erase(it);
                    callback(std::string(mReadMessage.body(), mReadMessage.body_length()));
                }
                catch (const std::exception & exc)
                {
                    std::cerr << "Caught exception from the client callback: " << exc.what() << std::endl;
                }
            }
            else
            {
                std::cerr << "Client received a response with unknown message id." << std::endl;
            }
            std::cout << "Number of pending requests: " << mCallbacks.size() << std::endl;
            async_read(mSocket,
                       buffer(mReadMessage.header(), mReadMessage.header_length()),
                       bind(&MessageClient::handle_read_header, this,
                            placeholders::error));
        }
        else
        {
            do_close();
        }
    }

    void do_write(const Message & msg)
    {
        bool write_in_progress = !mWriteMessages.empty();
        mWriteMessages.push_back(msg);
        if (!write_in_progress)
        {
            const Message & currentMessage = mWriteMessages.front();
            std::cout << "Writing to socket message with id " << mWriteMessages.front().get_id() << std::endl;
            async_write(mSocket,
                        buffer(currentMessage.data(),
                               currentMessage.length()),
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
                const Message & currentMessage = mWriteMessages.front();
                async_write(mSocket,
                            buffer(currentMessage.data(),
                                   currentMessage.length()),
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
    typedef std::map<uint32_t, ClientCallback> Callbacks;
    Callbacks mCallbacks;
};


} // MessageProtocol
