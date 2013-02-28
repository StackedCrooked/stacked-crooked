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


boost::asio::io_service gIOService;


std::vector<char> MakeMessage(const std::string & msg)
{
    uint32_t network_encoded_length = htonl(msg.size());
    char * length_buffer = reinterpret_cast<char *>(&network_encoded_length);
    std::vector<char> result;
    result.insert(result.end(), length_buffer, length_buffer + sizeof(network_encoded_length));
    result.insert(result.end(), msg.begin(), msg.end());
    return result;
}


class TCPSession
{
public:
    typedef std::function<std::string(TCPSession *, const std::string &)> RequestHandler;
    
    TCPSession(boost::asio::io_service & io_service, const RequestHandler & inRequestHandler) :
        mTCPSocket(io_service),
        mMessage(),
        mRequestHandler(inRequestHandler)
    {
        mMessage.reserve(1 * 1024 * 1024);
    }

    boost::asio::ip::tcp::socket & socket()
    {
        return mTCPSocket;
    }

    void start()
    {
        uint32_t network_encoded_length;
        char * length_buffer = reinterpret_cast<char *>(&network_encoded_length);
        boost::asio::read(mTCPSocket, boost::asio::buffer(length_buffer, sizeof(network_encoded_length)));
        std::string payload;
        payload.resize(ntohl(network_encoded_length));
        std::cout << "payload length: " << payload.size() << std::endl;
        if (boost::asio::read(mTCPSocket, boost::asio::buffer(&payload[0], payload.size())) != payload.size())
        {
            throw std::runtime_error("Not all bytes were read.");
        }
        std::cout << "payload: " << payload << std::endl;
        
        std::string response;
        
        try
        {
            response = mRequestHandler(this, payload);
        }
        catch (const std::exception & exc)
        {
            response = exc.what();
        }
        
        std::vector<char> response_msg = MakeMessage(response);

        if (boost::asio::write(mTCPSocket, boost::asio::buffer(&response_msg[0], response_msg.size())) != response_msg.size())
        {
            throw std::runtime_error("Not all bytes were written.");
        }
    }

private:
    boost::asio::ip::tcp::socket mTCPSocket;
    std::string mMessage;
    RequestHandler mRequestHandler;
};


typedef std::shared_ptr<TCPSession> TCPSessionPtr;


class TCPServer
{
public:
    typedef std::function<std::string(TCPSession *, const std::string &)> RequestHandler;
    
    TCPServer(short port, const RequestHandler & inRequestHandler) :
        mTCPAcceptor(gIOService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        mRequestHandler(inRequestHandler)
    {
        accept_connection();
    }

    void accept_connection()
    {
        TCPSessionPtr sessionPtr(new TCPSession(gIOService, mRequestHandler));
        mTCPAcceptor.async_accept(sessionPtr->socket(),
                                  boost::bind(&TCPServer::handle_new_connection,
                                              this,
                                              sessionPtr,
                                              boost::asio::placeholders::error));
    }

    void handle_new_connection(TCPSessionPtr session,
                               const boost::system::error_code & error)
    {
        if (!error)
        {
            session->start();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        accept_connection();
    }

private:
    boost::asio::ip::tcp::acceptor mTCPAcceptor;
    RequestHandler mRequestHandler;
};


using boost::asio::ip::tcp;


class TCPClient
{
public:
    TCPClient(const std::string & host, short port) :
        socket_(gIOService)
    {
        socket_.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(host), port));
    }

    std::string send(const std::string & message)
    {
        uint32_t network_encoded_length = htonl(message.size());
        {
            std::vector<char> request;

            request.insert(request.end(),
                           reinterpret_cast<char *>(&network_encoded_length),
                           reinterpret_cast<char *>(&network_encoded_length) + sizeof(network_encoded_length));
            request.insert(request.end(),
                           &message[0],
                           &message[0] + message.size());
            
            std::cout << "Writing " << request.size() << " bytes" << std::endl;

            boost::asio::write(socket_, boost::asio::buffer(&request[0], request.size()));
        }
        {
            boost::asio::read(socket_,
                              boost::asio::buffer(reinterpret_cast<char *>(&network_encoded_length),
                                                  sizeof(network_encoded_length)));
            std::string response;
            response.resize(ntohl(network_encoded_length));
            std::cout << "response length: " << response.size() << std::endl;
            if (boost::asio::read(socket_, boost::asio::buffer(&response[0], response.size())) != response.size())
            {
                throw std::runtime_error("Not all bytes were read.");
            }
            std::cout << "response: " << response << std::endl;
            return response;
        }
    }

private:
    tcp::socket socket_;
};


int main(int argc, char ** argv)
{
    std::vector<std::string> args(argv, argv + argc);
    try
    {
        if (args.at(1) == "server")
        {
            TCPServer server(9999, [](TCPSession * , std::string req) -> std::string {
                std::reverse(req.begin(), req.end());
                return req;
            });
            gIOService.run();
        }
        else
        {            
            TCPClient client("127.0.0.1", 9999);
            client.send("abc");
            gIOService.run();
        }
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
