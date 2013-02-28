#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <set>
#include <thread>
#include <boost/bind.hpp>
#include <boost/asio.hpp>


class MessageSession;
typedef std::function<std::string(MessageSession&, const std::string &)> RequestHandler;

struct Message
{
    template<typename Socket>
    static std::string Read(Socket & socket)
    {
        using namespace boost::asio;        
        std::string payload(Message::get_length(socket), 0);
        if (payload.size() != read(socket, buffer(&payload[0], payload.size())))
        {
            throw std::runtime_error("Not all bytes were read.");
        }
        return payload;
    }
    
    template<typename Socket>
    static void Write(Socket & socket, std::string payload)
    {
        using namespace boost::asio;
        uint32_t length_ne = htonl(payload.size());        
        payload.insert(payload.begin(), reinterpret_cast<char*>(&length_ne), reinterpret_cast<char*>(&length_ne) + sizeof(length_ne));
        if (payload.size() != write(socket, buffer(&payload[0], payload.size())))
        {
            throw std::runtime_error("Not all bytes were written.");
        }
    }
    
private:
    template<typename Socket>
    static uint32_t get_length(Socket & socket)
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
};


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
        Message::Write(socket_, mRequestHandler(*this, Message::Read(socket_)));
    }
    
    boost::asio::ip::tcp::socket & socket() { return socket_; }

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
            MessageSessionPtr sessionPtr(new MessageSession(io_service_, mRequestHandler));
            acceptor_.accept(sessionPtr->socket());
            sessionPtr->start();
        }
    }

private:
    boost::asio::io_service & io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    RequestHandler mRequestHandler;
};


using boost::asio::ip::tcp;


class MessageClient
{
public:
    MessageClient(boost::asio::io_service & io_serv, const std::string & host, short port) :
        io_service_(io_serv),
        socket_(io_service_)
    {
        socket_.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(host), port));
    }

    std::string send(std::string message)
    {
        Message::Write(socket_, message);
        return Message::Read(socket_);
    }

private:
    boost::asio::io_service & io_service_;
    tcp::socket socket_;
};


int main(int argc, char ** argv)
{
    std::vector<std::string> args(argv, argv + argc);
    try
    {
        boost::asio::io_service io_serv;
        if (args.at(1) == "server")
        {
            MessageServer server(io_serv, 9999, [](MessageSession& , std::string req) -> std::string {
                std::reverse(req.begin(), req.end());
                return req;
            });
            io_serv.run();
        }
        else
        {            
            MessageClient client(io_serv, "127.0.0.1", 9999);
            std::cout << client.send("123456789") << std::endl;
        }
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
