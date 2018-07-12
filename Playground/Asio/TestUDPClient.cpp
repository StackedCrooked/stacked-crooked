#include "Networking.h"
#include <boost/lexical_cast.hpp>
#include <iostream>


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl;
        return 1;
    }

    std::cout << "Creating client" << std::endl;
    UDPClient client(argv[1], boost::lexical_cast<int>(argv[2]));
    std::cout << "Starting client" << std::endl;
    client.send("X");

    std::cout << "End of program" << std::endl;
}
