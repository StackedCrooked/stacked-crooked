#include "Message.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <unistd.h>


namespace MessageProtocol {


using namespace boost::asio;
using boost::bind;
typedef std::deque<Message> RequestQueue;







class MessageSession : public boost::enable_shared_from_this<MessageSession>
{
public:
    MessageSession(io_service & io_service) :
        mSocket(io_service)
    {
    }

    ~MessageSession()
    {
    }

    ip::tcp::socket & socket()
    {
        return mSocket;
    }

    void start(int remaining = 100)
    {
        mRemaining = remaining;

        write_next();
    }

    void write_next()
    {
        if (mRemaining-- == 0)
        {
            return;
        }

        mReply.mRequest = std::chrono::steady_clock::now().time_since_epoch().count();

        async_write(
            mSocket,
            buffer(&mReply, sizeof(mReply)),
            bind(&MessageSession::handle_written, shared_from_this(), placeholders::error));
    }

    void handle_written(boost::system::error_code error)
    {
        if (error)
        {
            return;
        }

        read_reply();
    }

    void read_reply()
    {
        async_read(mSocket,
                   buffer(&mReply, sizeof(mReply)),
                   bind(&MessageSession::do_read, shared_from_this(), placeholders::error));
    }

    void do_read(const boost::system::error_code & error)
    {
        if (error)
        {
            return;
        }

        auto rtt = std::chrono::steady_clock::now().time_since_epoch().count() - mReply.mRequest;

        auto timediff = mReply.mRequest - mReply.mRequest + rtt/2;

        std::cout << "t1=" << mReply.mRequest << " t2=" << mReply.mReply << " rtt=" << rtt << " timediff=" << timediff << std::endl;

        write_next();
    }


private:
    ip::tcp::socket mSocket;
    int mRemaining = 0;

    struct Reply
    {
        int64_t mRequest;
        int64_t mReply;
    };

    Reply mReply;

};


typedef boost::shared_ptr<MessageSession> SessionPtr;


class MessageServer
{
public:
    MessageServer(uint16_t port) :
        mIOService(get_io_service()),
        mAcceptor(mIOService, ip::tcp::endpoint(ip::tcp::v4(), port))
    {
        start_accept();
        mIOService.run();
    }

    void start_accept()
    {
        auto new_session = boost::make_shared<MessageSession>(mIOService);

        mAcceptor.async_accept(
            new_session->socket(),
            bind(&MessageServer::handle_accept, this, new_session, placeholders::error));
    }

    void handle_accept(SessionPtr session,
                       const boost::system::error_code & error)
    {
        if (!error)
        {
            session->start();
        }

        start_accept();
    }

private:
    io_service & mIOService;
    ip::tcp::acceptor mAcceptor;
};


typedef boost::shared_ptr<MessageServer> ServerPtr;
typedef std::list<ServerPtr> Servers;


} // MessageProtocol
