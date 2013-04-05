#ifndef MESSAGEPROTOCOL_H
#define MESSAGEPROTOCOL_H


#include "Logging.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <array>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <stdexcept>




class MessageSession;
typedef std::function<std::string(MessageSession &, const std::string &)> RequestHandler;


namespace Detail {


typedef boost::asio::ip::tcp::socket Socket;


template<typename Socket>
uint32_t get_length(Socket & socket)
{
    using namespace boost::asio;

    // network-encoded length
    uint32_t length_ne;
    if (sizeof(length_ne) != read(socket, buffer(&length_ne, sizeof(length_ne))))
    {
        throw std::runtime_error("Failed to read message length.");
    }

    // return host-encoded
    return ntohl(length_ne);
}


std::string do_read(Socket & socket)
{
    using namespace boost::asio;
    std::string payload;
    payload.resize(Detail::get_length(socket), 0);
    if (payload.size() != read(socket, buffer(&payload[0], payload.size())))
    {
        throw std::runtime_error("Not all bytes were read.");
    }
    return payload;
}


void do_write(Socket & socket, std::string payload)
{
    using namespace boost::asio;
    uint32_t length_ne = htonl(payload.size());
    payload.insert(payload.begin(), reinterpret_cast<char *>(&length_ne), reinterpret_cast<char *>(&length_ne) + sizeof(length_ne));
    if (payload.size() != write(socket, buffer(&payload[0], payload.size())))
    {
        throw std::runtime_error("Not all bytes were written.");
    }
}


std::string Read(Socket & socket)
{
    try
    {
        return do_read(socket);
    }
    catch (boost::exception & exc)
    {
        std::cout << "Message::Read failed. Extra info: " << boost::diagnostic_information(exc);
        throw;
    }
}

void Write(Socket & socket, std::string payload)
{
    try
    {
        do_write(socket, payload);
    }
    catch (boost::exception & exc)
    {
        std::cout << "Message::Write failed. Extra info: " << boost::diagnostic_information(exc);
        throw;
    }
}


}


class MessageSession
{
public:
    MessageSession(boost::asio::io_service & io_serv, const RequestHandler & inRequestHandler) :
        io_service_(io_serv),
        socket_(io_service_),
        mRequestHandler(inRequestHandler)
    {
    }

    void start()
    {
        Detail::Write(socket_, mRequestHandler(*this, Detail::Read(socket_)));
    }

    boost::asio::ip::tcp::socket & socket()
    {
        return socket_;
    }

private:
    boost::asio::io_service & io_service_;
    boost::asio::ip::tcp::socket socket_;
    RequestHandler mRequestHandler;
};


typedef std::shared_ptr<MessageSession> MessageSessionPtr;


class MessageServer
{
public:
    MessageServer(boost::asio::io_service & io_serv, short port, const RequestHandler & inRequestHandler) :
        io_service_(io_serv),
        acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        mRequestHandler(inRequestHandler)
    {
        while (true)
        {
            try
            {
                MessageSessionPtr sessionPtr(new MessageSession(io_service_, mRequestHandler));
                acceptor_.accept(sessionPtr->socket());
                sessionPtr->start();
            }
            catch (boost::system::system_error & exc)
            {
                std::cout << "RPC session closed. Extra info: " << boost::diagnostic_information(exc);
            }
        }
    }

private:
    boost::asio::io_service & io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    RequestHandler mRequestHandler;
};


struct Message
{
    enum { HeaderLength = sizeof(uint32_t) };

    Message() : header_(), body_() { }
    
    const char * header() const { return &header_[0]; }
    char * header() { return &header_[0]; }    

    const char * body() const { return body_.data(); }
    char * body() { return &body_[0]; }
    
    unsigned body_length() const { return body_.size(); }
    
    void read_header()
    {
        uint32_t n;
        memcpy(&n, &header_[0], sizeof(n));
        body_.resize(ntohl(n));        
    }

    std::array<char, HeaderLength> header_;
    std::string body_;
};


class MessageClient
{
    typedef boost::asio::ip::tcp::resolver Resolver;
    typedef Resolver::iterator Iterator;
    typedef boost::system::error_code Error;

public:
    MessageClient(boost::asio::io_service & io_serv, const std::string & host, short port) :
        io_service_(io_serv),
        host_(host),
        port_(port),
        socket_(io_service_),
        message_()
    {
        Resolver resolver(io_service_);
        Resolver::query query(host, std::to_string(port));
        Iterator endpoint_iterator = resolver.resolve(query);
        auto endpoint = *endpoint_iterator;
        socket_.async_connect(endpoint, boost::bind(&MessageClient::handle_connect, this, boost::asio::placeholders::error, ++endpoint_iterator));
    }

    std::future<std::string> send(const std::string & msg)
    {
        Detail::Write(socket_, msg);
        return promised_result_.get_future();
    }

private:
    void handle_connect(const Error & error, Iterator it)
    {
        if (!error)
        {
            boost::asio::async_read(
                socket_,
                boost::asio::buffer(message_.header(), Message::HeaderLength),
                boost::bind(&MessageClient::handle_read_header, this, boost::asio::placeholders::error));
        }
        else if (it != Iterator())
        {
            socket_.close();
            auto endpoint = *it;
            socket_.async_connect(endpoint,
                                  boost::bind(&MessageClient::handle_connect, this,
                                            boost::asio::placeholders::error, ++it));
        }
    }

    void handle_read_header(const Error & error)
    {
        if (error)
        {
            socket_.close();
            return;
        }
        
        message_.read_header();

        boost::asio::async_read(
                    socket_,
                    boost::asio::buffer(message_.body(), message_.body_length()),
                    boost::bind(&MessageClient::handle_read_body, this, boost::asio::placeholders::error));
    }

    void handle_read_body(const boost::system::error_code& error)
    {
        if (error)
        {
            socket_.close();
            return;
        }
        
        promised_result_.set_value(message_.body());
        
        boost::asio::async_read(socket_,
                                boost::asio::buffer(message_.header(), Message::HeaderLength),
                                boost::bind(&MessageClient::handle_read_header,
                                          this,
                                          boost::asio::placeholders::error));
    }

    boost::asio::io_service & io_service_;
    std::string host_;
    short port_;
    boost::asio::ip::tcp::socket socket_;
    Message message_;
    std::promise<std::string> promised_result_;
    
};


#endif // MESSAGEPROTOCOL_H
