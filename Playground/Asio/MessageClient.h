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

    boost::asio::io_service& mIOService;
    ip::tcp::socket mSocket;

    struct Reply
    {
        int64_t mRequest;
        int64_t mReply;
    };
    Reply mReply;


    MessageClient(const std::string & host, uint16_t port) :
        mIOService(get_io_service()),
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



private:
    void handle_connect(const boost::system::error_code & error)
    {
        if (error)
        {
            std::cout << "handle_connect failed " << std::endl;
            std::cout << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << " error=" << error << std::endl;
            return;
        }

        read();

    }

    void read()
    {
        async_read(mSocket,
                   buffer(&mReply, sizeof(mReply)),
                   bind(&MessageClient::handle_read, this, placeholders::error));
    }


    void handle_read(const boost::system::error_code & error)
    {
        if (error)
        {
            std::cout << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << std::endl;
            mSocket.close();
            return;
        }

        std::cerr << "mReply.mRequest=" << mReply.mRequest << std::endl;

        mReply.mReply = std::chrono::steady_clock::now().time_since_epoch().count();

        async_write(
            mSocket,
            buffer(&mReply, sizeof(mReply)),
            bind(&MessageClient::handle_written, this, placeholders::error));
    }

    void handle_written(boost::system::error_code error)
    {
        if (error)
        {
            std::cout << __FILE__ << ":" << __LINE__ << ": " << __FUNCTION__ << std::endl;
            return;
        }

        read();
    }
};


} // MessageProtocol
