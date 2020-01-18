#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::udp;


std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}


static constexpr auto port_number = 12345;


int main()
{
    try
    {
        std::cout << "Listening to " << port_number << std::endl;
        boost::asio::io_context io_context;

        udp::socket socket(io_context, udp::endpoint(udp::v4(), port_number));

        uint64_t num_received = 0;
        for (;;)
        {
            boost::array<char, 128 * 1024> recv_buf;
            udp::endpoint remote_endpoint;
            boost::system::error_code error;
            auto num_bytes = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);
            if (++num_received % 1000 == 0)
            {
                std::cout << "Received message " << ++num_received << ". Size=" << num_bytes << std::endl;
            }

            std::string message = make_daytime_string();

            boost::system::error_code ignored_error;
            socket.send_to(boost::asio::buffer(message),
                           remote_endpoint, 0, ignored_error);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
