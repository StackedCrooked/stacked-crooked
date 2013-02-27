#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>


using boost::asio::ip::tcp;


boost::asio::io_service & get_io_service()
{
    static boost::asio::io_service io;
    return io;
}


struct TCPServer::Impl
{
    Impl(uint16_t port) :
        acceptor_(get_io_service(),
                  tcp::endpoint(tcp::v4(), port))
    {
        while (true)
        {
            std::unique_ptr<tcp::socket> sock(new tcp::socket(get_io_service()));
            a.accept(*sock);
            std::thread t(std::bind(session, std::move(sock)));
        }
    }

    void session(std::unique_ptr<tcp::socket> sockptr)
    {
        try
        {
            for (;;)
            {
                char data[max_length];
                tcp::socket & sock = *sockptr;

                boost::system::error_code error;
                size_t length = sock.read_some(boost::asio::buffer(data), error);
                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer.
                else if (error)
                    throw boost::system::system_error(error); // Some other error.

                boost::asio::write(*sock, boost::asio::buffer(data, length));
            }
        }
        catch (std::exception & e)
        {
            std::cerr << "Exception in thread: " << e.what() << "\n";
        }
    }


    void start_accept()
    {
    }

    void handle_accept(Session * new_session,
                       const boost::system::error_code & error)
    {
        if (!error)
        {
            new_session->start();
        }
        else
        {
            delete new_session;
        }

        start_accept();
    }

    tcp::acceptor acceptor_;
};


TCPServer::TCPServer(uint16_t port) :
    mImpl(new Impl(port))
{
}



//int main(int argc, char * argv[])
//{
//    try
//    {
//        if (argc != 2)
//        {
//            std::cerr << "Usage: async_tcp_echo_server <port>\n";
//            return 1;
//        }

//        boost::asio::io_service io_service;

//        using namespace std; // For atoi.
//        TCPServer s(io_service, atoi(argv[1]));

//        io_service.run();
//    }
//    catch (std::exception & e)
//    {
//        std::cerr << "Exception: " << e.what() << "\n";
//    }

//    return 0;
//}
