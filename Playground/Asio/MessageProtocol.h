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
using namespace boost::asio::ip;
using namespace boost::asio::placeholders;
using namespace boost::system;


typedef ip::tcp::resolver Resolver;
typedef Resolver::iterator Iterator;
typedef error_code Error;


struct Message
{
    enum { HeaderLength = sizeof(uint32_t) };
    typedef std::array<char, HeaderLength> Header;

    Message(const std::string & str) : header_(Message::makeHeader(str)), body_(str) { }
    Message() : header_(), body_() { }

    const char * header() const
    {
        return &header_[0];
    }
    char * header()
    {
        return &header_[0];
    }

    const char * body() const
    {
        return body_.data();
    }
    char * body()
    {
        return &body_[0];
    }

    const std::string & get_body() const
    {
        return body_;
    }

    unsigned body_length() const
    {
        return body_.size();
    }

    void decode_header()
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


io_service & get_io_service()
{
    static io_service serv;
    return serv;
}



typedef std::function<std::string(std::string)> Callback;


class Session
{
public:
    Session(const Callback & callback) :
        socket_(get_io_service()),
        callback_(callback)
    {
    }

    tcp::socket & socket()
    {
        return socket_;
    }

    void start()
    {
        async_read(socket_,
                   buffer(read_msg_.header(), Message::HeaderLength),
                   boost::bind(&Session::handle_read_header,
                               this,
                               placeholders::error));
    }

    void deliver(const Message & msg)
    {
        bool write_in_progress = !write_queue_.empty();
        write_queue_.push_back(msg);
        if (!write_in_progress)
        {
            Message & msg = write_queue_.front();
            async_write(socket_,
                        buffer(msg.body(), msg.body_length()),
                        boost::bind(&Session::handle_write,
                                    this,
                                    placeholders::error));
        }
    }

    void handle_read_header(const error_code & error)
    {
        if (error)
        {
            return;
        }

        read_msg_.decode_header();
        async_read(socket_,
                   buffer(read_msg_.body(), read_msg_.body_length()),
                   boost::bind(&Session::handle_read_body, this, placeholders::error));
    }

    void handle_read_body(const error_code & error)
    {
        if (error)
        {
            return;
        }

        deliver(callback_(read_msg_.get_body()));
        async_read(socket_,
                   buffer(read_msg_.header(), read_msg_.HeaderLength),
                   boost::bind(&Session::handle_read_header,
                               this,
                               placeholders::error));
    }

    void handle_write(const error_code & error)
    {
        if (error)
        {
            return;
        }

        write_queue_.pop_front();
        if (!write_queue_.empty())
        {
            Message & msg = write_queue_.front();
            async_write(socket_,
                        buffer(msg.body(), msg.body_length()),
                        boost::bind(&Session::handle_write,
                                    this,
                                    placeholders::error));
        }
    }

private:
    tcp::socket socket_;
    Callback callback_;
    Message read_msg_;
    std::deque<Message> write_queue_;
};



typedef boost::shared_ptr<Session> SessionPtr;


class MessageServer
{
public:
    MessageServer(short port, const Callback & callback) :
        io_service_(get_io_service()),
        acceptor_(get_io_service(), tcp::endpoint(tcp::v4(), port)),
        callback_(callback)
    {
        start_accept();
        get_io_service().run();
    }

    void start_accept()
    {
        SessionPtr new_session(new Session(callback_));
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&MessageServer::handle_accept, this, new_session,
                                           placeholders::error));
    }

    void handle_accept(SessionPtr session,
                       const error_code & error)
    {
        if (!error)
        {
            session->start();
        }

        start_accept();
    }

private:
    io_service & io_service_;
    tcp::acceptor acceptor_;
    Callback callback_;
};


struct MessageClient
{
    MessageClient(const std::string & host, short port) :
        io_service_(get_io_service()),
        host_(host),
        port_(port),
        socket_(io_service_),
        message_()
    {
        Resolver resolver(io_service_);
        Resolver::query query(host, std::to_string(port));
        Iterator endpoint_iterator = resolver.resolve(query);
        auto endpoint = *endpoint_iterator;
        socket_.async_connect(endpoint,
                              bind(&MessageClient::handleConnect,
                                   this,
                                   placeholders::error,
                                   ++endpoint_iterator));
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

        message_.decode_header();

        async_read(socket_,
                   buffer(message_.body(), message_.body_length()),
                   bind(&MessageClient::readBody, this, error));
    }

    void readBody(const error_code & error)
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


#endif // MESSAGEPROTOCOL_H
