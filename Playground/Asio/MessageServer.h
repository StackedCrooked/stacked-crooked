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
        std::cout << "join" << std::endl;
        mSessions.insert(session);
        std::for_each(mRequestQueue.begin(), mRequestQueue.end(),
                      boost::bind(&AbstractMessageSession::deliver, session, _1));
    }

    void leave(AbstractSessionPtr participant)
    {
        std::cout << "leave" << std::endl;
        mSessions.erase(participant);
    }

    void deliver(const Message & msg)
    {
        mRequestQueue.push_back(msg);
        while (mRequestQueue.size() > max_recent_msgs)
            mRequestQueue.pop_front();

        std::for_each(mSessions.begin(), mSessions.end(),
                      boost::bind(&AbstractMessageSession::deliver, _1, boost::ref(msg)));
    }

private:
    std::set<AbstractSessionPtr> mSessions;
    enum { max_recent_msgs = 100 };
    RequestQueue mRequestQueue;
};


class MessageSession : public AbstractMessageSession,
                       public boost::enable_shared_from_this<MessageSession>
{
public:
    MessageSession(boost::asio::io_service & io_service, MessageSessions & room) :
        socket_(io_service),
        mSessions(room),
        read_msg_()
    {
        std::cout << "MessageSession created." << std::endl;
    }
    
    ~MessageSession()
    {
        std::cout << "MessageSession destroyed." << std::endl;
    }

    boost::asio::ip::tcp::socket & socket()
    {
        return socket_;
    }

    void start()
    {
        mSessions.join(shared_from_this());
        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_.data(), Message::header_length),
                                boost::bind(&MessageSession::handle_read_header, shared_from_this(),
                                            boost::asio::placeholders::error));
    }

    void deliver(const Message & msg)
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            boost::asio::async_write(socket_,
                                     boost::asio::buffer(write_msgs_.front().data(),
                                                         write_msgs_.front().length()),
                                     boost::bind(&MessageSession::handle_write, shared_from_this(),
                                                 boost::asio::placeholders::error));
        }
    }

    void handle_read_header(const boost::system::error_code & error)
    {
        if (!error && read_msg_.decode_header())
        {
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                                    boost::bind(&MessageSession::handle_read_body, shared_from_this(),
                                                boost::asio::placeholders::error));
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
            mSessions.deliver(read_msg_);
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(read_msg_.data(), Message::header_length),
                                    boost::bind(&MessageSession::handle_read_header, shared_from_this(),
                                                boost::asio::placeholders::error));
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
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
                boost::asio::async_write(socket_,
                                         boost::asio::buffer(write_msgs_.front().data(),
                                                             write_msgs_.front().length()),
                                         boost::bind(&MessageSession::handle_write, shared_from_this(),
                                                     boost::asio::placeholders::error));
            }
        }
        else
        {
            mSessions.leave(shared_from_this());
        }
    }

private:
    boost::asio::ip::tcp::socket socket_;
    MessageSessions & mSessions;
    Message read_msg_;
    RequestQueue write_msgs_;
};


typedef boost::shared_ptr<MessageSession> SessionPtr;


class MessageServer
{
public:
    MessageServer(boost::asio::io_service & io_service,
                  const boost::asio::ip::tcp::endpoint & endpoint)
        : io_service_(io_service),
          acceptor_(io_service, endpoint)
    {
        start_accept();
    }

    void start_accept()
    {
        SessionPtr new_session(new MessageSession(io_service_, room_));
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&MessageServer::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
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
    boost::asio::io_service & io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    MessageSessions room_;
};


typedef boost::shared_ptr<MessageServer> ServerPtr;
typedef std::list<ServerPtr> Servers;
