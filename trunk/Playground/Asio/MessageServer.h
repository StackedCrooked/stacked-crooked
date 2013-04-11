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
#include <unistd.h>


namespace MessageProtocol {


using namespace boost::asio;
using boost::bind;
typedef std::deque<Message> RequestQueue;


class AbstractMessageSession;


typedef std::function<std::string(AbstractMessageSession&, const std::string&)> ServerCallback;


class AbstractMessageSession
{
public:    
    AbstractMessageSession(const ServerCallback & inCallback) :
        mCallback(inCallback)
    {
    }
    
    virtual ~AbstractMessageSession() {}
    
    std::string execute(const std::string & str)
    {
        return mCallback(*this, str);
    }
        
private:
    ServerCallback mCallback;
};


typedef boost::shared_ptr<AbstractMessageSession> AbstractSessionPtr;


class MessageSessions
{
public:
    
    void join(AbstractSessionPtr session)
    {
        std::cout << "join" << std::endl;
        mSessions.insert(session);
    }

    void leave(AbstractSessionPtr participant)
    {
        std::cout << "leave" << std::endl;
        mSessions.erase(participant);
    }
    
private:
    std::set<AbstractSessionPtr> mSessions;
};


class MessageSession : public AbstractMessageSession,
                       public boost::enable_shared_from_this<MessageSession>
{
public:
    MessageSession(io_service & io_service, MessageSessions & room, const ServerCallback & inServerCallback) :
        AbstractMessageSession(inServerCallback),
        mSocket(io_service),
        mSessions(room),
        mReadMessage("")
    {
    }
    
    ~MessageSession()
    {
    }

    ip::tcp::socket & socket()
    {
        return mSocket;
    }

    void start()
    {
        mSessions.join(shared_from_this());
        read_header();
    }
    
    void read_header()
    {        
        async_read(mSocket,
                   buffer(mReadMessage.header(), mReadMessage.header_length()),
                   bind(&MessageSession::handle_read_header, shared_from_this(), placeholders::error));
    }

    void handle_read_header(const boost::system::error_code & error)
    {
        if (!error)
        {
            mReadMessage.decode_header();
            read_body();
        }
        else
        {
            mSessions.leave(shared_from_this());
        }
    }
    
    void read_body()
    {        
        async_read(mSocket,
                   buffer(mReadMessage.body(), mReadMessage.body_length()),
                   bind(&MessageSession::handle_read_body, shared_from_this(), placeholders::error));
    }

    void handle_read_body(const boost::system::error_code & error)
    {
        if (!error)
        {
            write(mReadMessage.get_id(), execute(mReadMessage.copy_body()));
            read_header();
        }
        else
        {
            mSessions.leave(shared_from_this());
        }
    }
    
    void write(uint32_t inId, const std::string & inString)
    {
        Message msg(inString);
        msg.set_id(inId);
        write(msg);
        
    }
    
    void write(const Message & inMessage)
    {        
        async_write(mSocket,
                    buffer(inMessage.header(), inMessage.length()),
                    bind(&MessageSession::handle_write, shared_from_this(), placeholders::error));
    }

    void handle_write(const boost::system::error_code & error)
    {
        if (!error)
        {
            if (!mRequestQueue.empty())
            {
                auto msg = mRequestQueue.front();
                mRequestQueue.pop_front();
                write(msg);
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
};


typedef boost::shared_ptr<MessageSession> SessionPtr;


class MessageServer
{
public:
    MessageServer(uint16_t port, const ServerCallback & inServerCallback) :
        mIOService(get_io_service()),
        mAcceptor(mIOService, ip::tcp::endpoint(ip::tcp::v4(), port)),
        mServerCallback(inServerCallback)
    {        
        start_accept();
        mIOService.run();
    }

    void start_accept()
    {
        SessionPtr new_session(new MessageSession(mIOService, mSessions, mServerCallback));
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
    ServerCallback mServerCallback;
};


typedef boost::shared_ptr<MessageServer> ServerPtr;
typedef std::list<ServerPtr> Servers;


} // MessageProtocol
