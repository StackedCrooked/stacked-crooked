#include "Commands.h"
#include "Networking.h"
#include <iostream>


using namespace boost::tuples;


int main()
{
    UDPClient client("127.0.0.1", 9001);
    tuple<std::string, std::string> arg =
        make_tuple(CreateStopwatch::CommandName(), "Stoppy001");
    std::cout << client.send(serialize(arg)) << std::endl;
}
