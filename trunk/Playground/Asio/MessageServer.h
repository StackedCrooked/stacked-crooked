#include "Message.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>


namespace MessageProtocol {


using namespace boost::asio;
using boost::bind;
typedef std::deque<Message> RequestQueue;


class AbstractMessageSession
{
public:
    virtual ~AbstractMessageSession() {}
    virtual void deliver(const Message & msg) = 0;
};


typedef boost::shared_ptr<AbstractMessageSession> AbstractSessionPtr;


class MessageSessions
{
public:
    void join(AbstractSessionPtr session)
    {
        mSessions.insert(session);
    }

    void leave(AbstractSessionPtr participant)
    {
        mSessions.erase(participant);
    }
    
private:
    std::set<AbstractSessionPtr> mSessions;
};


class MessageSession : public AbstractMessageSession,
                       public boost::enable_shared_from_this<MessageSession>
{
public:
    typedef std::function<std::string(const std::string&)> Callback;
    
    MessageSession(io_service & io_service, MessageSessions & room) :
        mSocket(io_service),
        mSessions(room),
        mReadMessage(""),
        mCallback()
    {
    }
    
    ~MessageSession()
    {
    }
    
    void setCallback(const Callback & inCallback)
    {
        mCallback = inCallback;
    }

    ip::tcp::socket & socket()
    {
        return mSocket;
    }

    void start()
    {
        mSessions.join(shared_from_this());
        async_read(mSocket,
                   buffer(mReadMessage.header(), mReadMessage.header_length()),
                   bind(&MessageSession::handle_read_header, shared_from_this(),
                        placeholders::error));
    }

    void deliver(const Message & msg)
    {
        bool write_in_progress = !mRequestQueue.empty();
        mRequestQueue.push_back(msg);
        if (!write_in_progress)
        {
            const Message & currentMessage = mRequestQueue.front();
            async_write(mSocket,
                        buffer(currentMessage.header(), currentMessage.length()),
                        bind(&MessageSession::handle_write, shared_from_this(), placeholders::error));
        }
    }

    void handle_read_header(const boost::system::error_code & error)
    {
        if (!error)
        {
            mReadMessage.decode_header();
            async_read(mSocket,
                       buffer(mReadMessage.body(), mReadMessage.body_length()),
                       bind(&MessageSession::handle_read_body, shared_from_this(), placeholders::error));
        }
        else
        {
            mSessions.leave(shared_from_this());
        }
    }

    void handle_read_body(const boost::system::error_code & error)
    {
        if (!error)
        {
            Message reply(mCallback(std::string(mReadMessage.body(), mReadMessage.body() + mReadMessage.body_length())));            
            reply.set_id(mReadMessage.get_id());
            deliver(Message(reply));
            async_read(mSocket,
                       buffer(mReadMessage.header(), mReadMessage.header_length()),
                       bind(&MessageSession::handle_read_header, shared_from_this(), placeholders::error));
        }
        else
        {
            mSessions.leave(shared_from_this());
        }
    }

    void handle_write(const boost::system::error_code & error)
    {
        if (!error)
        {
            mRequestQueue.pop_front();
            if (!mRequestQueue.empty())
            {
                const Message & currentMessage = mRequestQueue.front();
                async_write(mSocket,
                            buffer(currentMessage.header(), currentMessage.length()),
                            bind(&MessageSession::handle_write, shared_from_this(), placeholders::error));
            }
        }
        else
        {
            mSessions.leave(shared_from_this());
        }
    }

private:
    ip::tcp::socket mSocket;
    MessageSessions & mSessions;
    Message mReadMessage;
    RequestQueue mRequestQueue;
    Callback mCallback;
};


typedef boost::shared_ptr<MessageSession> SessionPtr;


class MessageServer
{
public:
    typedef std::function<std::string(const MessageSession&, const std::string&)> Callback;
    
    MessageServer(uint16_t port, const Callback & inCallback) :
        mIOService(get_io_service()),
        mAcceptor(mIOService, ip::tcp::endpoint(ip::tcp::v4(), port)),
        mCallback(inCallback)
    {        
        start_accept();
        mIOService.run();
    }

    void start_accept()
    {
        SessionPtr new_session(new MessageSession(mIOService, mSessions));
        new_session->setCallback([=](const std::string & response) {
            return mCallback(*new_session, response);
        });
        mAcceptor.async_accept(new_session->socket(),
                               bind(&MessageServer::handle_accept, this, new_session,
                                           placeholders::error));
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
    MessageSessions mSessions;
    Callback mCallback;
};


typedef boost::shared_ptr<MessageServer> ServerPtr;
typedef std::list<ServerPtr> Servers;


} // MessageProtocol
