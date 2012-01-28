#include "Client/UDPClient.h"
#include <iostream>


int main()
{
    Client::UDPClient client("127.0.0.1", 9001);
    std::cout << client.send("Hello") << std::endl;
}
