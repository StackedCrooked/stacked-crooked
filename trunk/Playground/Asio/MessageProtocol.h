#ifndef MESSAGEPROTOCOL_H
#define MESSAGEPROTOCOL_H


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <array>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <stdexcept>


namespace Asio {


using namespace boost;
using namespace boost::asio;
using namespace boost::asio::placeholders;
using namespace boost::system;
using boost::asio::placeholders::error;


typedef ip::tcp::resolver Resolver;
typedef Resolver::iterator Iterator;
typedef error_code Error;


class MessageSession;
typedef std::function<std::string(MessageSession &, const std::string &)> RequestHandler;


namespace Detail {


typedef ip::tcp::socket Socket;


template<typename Socket>
uint32_t get_length(Socket & socket)
{
    using namespace asio;

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
    using namespace asio;
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
    using namespace asio;
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
    catch (exception & exc)
    {
        std::cout << "Message::Read failed. Extra info: " << diagnostic_information(exc);
        throw;
    }
}

void Write(Socket & socket, std::string payload)
{
    try
    {
        do_write(socket, payload);
    }
    catch (exception & exc)
    {
        std::cout << "Message::Write failed. Extra info: " << diagnostic_information(exc);
        throw;
    }
}


}


class MessageSession
{
public:
    MessageSession(io_service & io_serv, const RequestHandler & inRequestHandler) :
        io_service_(io_serv),
        socket_(io_service_),
        mRequestHandler(inRequestHandler)
    {
    }

    void start()
    {
        Detail::Write(socket_, mRequestHandler(*this, Detail::Read(socket_)));
    }

    ip::tcp::socket & socket()
    {
        return socket_;
    }

private:
    io_service & io_service_;
    ip::tcp::socket socket_;
    RequestHandler mRequestHandler;
};


typedef std::shared_ptr<MessageSession> MessageSessionPtr;


class MessageServer
{
public:
    MessageServer(io_service & io_serv, short port, const RequestHandler & inRequestHandler) :
        io_service_(io_serv),
        acceptor_(io_service_, ip::tcp::endpoint(ip::tcp::v4(), port)),
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
            catch (system_error & exc)
            {
                std::cout << "RPC session closed. Extra info: " << diagnostic_information(exc);
            }
        }
    }

private:
    io_service & io_service_;
    ip::tcp::acceptor acceptor_;
    RequestHandler mRequestHandler;
};


struct Message
{
    enum { HeaderLength = sizeof(uint32_t) };
    typedef std::array<char, HeaderLength> Header;

    Message(const std::string & str) : header_(Message::makeHeader(str)), body_(str) { }
    Message() : header_(), body_() { }
    
    const char * header() const { return &header_[0]; }
    char * header() { return &header_[0]; }    

    const char * body() const { return body_.data(); }
    char * body() { return &body_[0]; }
    
    unsigned body_length() const { return body_.size(); }

    void read_header()
    {
        body_.resize(Message::parseHeader(header_));
    }

    static unsigned parseHeader(const Header & hdr)
    {
        uint32_t n;
        memcpy(&n, &hdr[0], sizeof(n));
        return ntohl(n);
    }

    static Header makeHeader(const std::string & str)
    {
        Header result;
        unsigned n = htonl(unsigned(str.size()));
        memcpy(&result, &n, sizeof(result));
        return result;
    }

    Header header_;
    std::string body_;
};


struct MessageClient
{
    MessageClient(io_service & io_serv, const std::string & host, short port) :
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
        socket_.async_connect(endpoint, bind(&MessageClient::handleConnect, this, error, ++endpoint_iterator));
    }

    std::future<std::string> send(const std::string & msg)
    {
        write(Message(msg));
        return promised_result_.get_future();
    }

    void close()
    {
        socket_.close();
    }

private:
    void handleConnect(const Error & error, Iterator it)
    {
        if (!error)
        {
            async_read(socket_,
                       buffer(message_.header(), Message::HeaderLength),
                       bind(&MessageClient::readHeader, this, error));
        }
        else if (it != Iterator())
        {
            socket_.close();
            auto endpoint = *it;
            socket_.async_connect(endpoint,
                                  bind(&MessageClient::handleConnect, this, error, ++it));
        }
    }

    void readHeader(const Error & error)
    {
        if (error)
        {
            socket_.close();
            return;
        }
        
        message_.read_header();

        async_read(socket_,
                   buffer(message_.body(), message_.body_length()),
                   bind(&MessageClient::readBody, this, error));
    }

    void readBody(const error_code& error)
    {
        if (error)
        {
            socket_.close();
            return;
        }
        
        promised_result_.set_value(message_.body());
        
        async_read(socket_,
                   buffer(message_.header(), Message::HeaderLength),
                   bind(&MessageClient::readHeader, this, error));
    }

    void write(const Message & msg)
    {
        bool write_in_progress = !write_queue_.empty();
        write_queue_.push_back(msg);
        if (!write_in_progress)
        {
            async_write(socket_,
                        buffer(write_queue_.front().body(), write_queue_.front().body_length()),
                        bind(&MessageClient::handleWrite, this, placeholders::error));
        }
    }

    void handleWrite(const system::error_code & error)
    {
        if (error)
        {
            close();
            return;
        }

        write_queue_.pop_front();
        if (!write_queue_.empty())
        {
            Message & msg = write_queue_.front();
            async_write(socket_,
                        buffer(msg.body(), msg.body_length()),
                        bind(&MessageClient::handleWrite, this, placeholders::error));
        }
    }

    io_service & io_service_;
    std::string host_;
    short port_;
    ip::tcp::socket socket_;
    Message message_;
    std::deque<Message> write_queue_;
    std::promise<std::string> promised_result_;
    
};

} // Asio


using Asio::MessageClient;
using Asio::MessageServer;


#endif // MESSAGEPROTOCOL_H
