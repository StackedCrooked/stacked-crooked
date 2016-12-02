#include <boost/asio.hpp>
#include <iostream>

boost::asio::io_service ios;

int main()
{
    std::cout << "Start of program" << std::endl;
    ios.post([]{ std::cout << "POSTED" << std::endl; });
    ios.run();
    std::cout << "End of program." << std::endl;
}
