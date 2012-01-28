#include "Commands.h"
#include "Networking.h"
#include <iostream>


int main()
{
    UDPClient client("127.0.0.1", 9001);
    std::cout << client.send(serialize(CreateStopwatch("stoppy"))) << std::endl;
}
