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


using boost::asio::ip::tcp;


typedef std::deque<Message> RequestQueue;


class AbstractSession
{
public:
    virtual ~AbstractSession() {}
    virtual void deliver(const Message & msg) = 0;
};


typedef boost::shared_ptr<AbstractSession> AbstractSessionPtr;


class Sessions
{
public:
    void join(AbstractSessionPtr session)
    {
        std::cout << "join" << std::endl;
        mSessions.insert(session);
        std::for_each(mRequestQueue.begin(), mRequestQueue.end(),
                      boost::bind(&AbstractSession::deliver, session, _1));
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
                      boost::bind(&AbstractSession::deliver, _1, boost::ref(msg)));
    }

private:
    std::set<AbstractSessionPtr> mSessions;
    enum { max_recent_msgs = 100 };
    RequestQueue mRequestQueue;
};


class Session : public AbstractSession,
                public boost::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::io_service & io_service, Sessions & room) :
        socket_(io_service),
        room_(room)
    {
    }

    tcp::socket & socket()
    {
        return socket_;
    }

    void start()
    {
        room_.join(shared_from_this());
        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_.data(), Message::header_length),
                                boost::bind(&Session::handle_read_header, shared_from_this(),
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
                                     boost::bind(&Session::handle_write, shared_from_this(),
                                                 boost::asio::placeholders::error));
        }
    }

    void handle_read_header(const boost::system::error_code & error)
    {
        if (!error && read_msg_.decode_header())
        {
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                                    boost::bind(&Session::handle_read_body, shared_from_this(),
                                                boost::asio::placeholders::error));
        }
        else
        {
            room_.leave(shared_from_this());
        }
    }

    void handle_read_body(const boost::system::error_code & error)
    {
        if (!error)
        {
            room_.deliver(read_msg_);
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(read_msg_.data(), Message::header_length),
                                    boost::bind(&Session::handle_read_header, shared_from_this(),
                                                boost::asio::placeholders::error));
        }
        else
        {
            room_.leave(shared_from_this());
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
                                         boost::bind(&Session::handle_write, shared_from_this(),
                                                     boost::asio::placeholders::error));
            }
        }
        else
        {
            room_.leave(shared_from_this());
        }
    }

private:
    tcp::socket socket_;
    Sessions & room_;
    Message read_msg_;
    RequestQueue write_msgs_;
};

typedef boost::shared_ptr<Session> SessionPtr;



class Server
{
public:
    Server(boost::asio::io_service & io_service,
                const tcp::endpoint & endpoint)
        : io_service_(io_service),
          acceptor_(io_service, endpoint)
    {
        start_accept();
    }

    void start_accept()
    {
        SessionPtr new_session(new Session(io_service_, room_));
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&Server::handle_accept, this, new_session,
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
    tcp::acceptor acceptor_;
    Sessions room_;
};

typedef boost::shared_ptr<Server> ServerPtr;
typedef std::list<ServerPtr> Servers;
