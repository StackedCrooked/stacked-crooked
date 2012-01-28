#include <boost/asio.hpp>
#include <iostream>


using namespace boost::asio;


int main()
{
    io_service ios;
    ip::tcp::endpoint endpoint(ip::tcp::v4(), 9001);
    ip::tcp::acceptor acceptor(ios, endpoint);

    std::cout << "Listening on " << endpoint.port() << std::endl;
    while (true)
    {
        ip::tcp::iostream stream;
        acceptor.accept(*stream.rdbuf());
        std::string request;
        stream >> request;
        std::cout << "request: " << request << std::endl;
    }
}
