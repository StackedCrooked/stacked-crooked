#include "MessageClient.h"
#include <iostream>
#include <thread>


int main(int argc, char** argv)
{
    const char* hostname = "127.0.0.1";
    if (argc >= 2)
    {
        hostname = argv[1];
    }

    std::cout << "HOSTNAME=" << hostname << std::endl;

    MessageProtocol::MessageClient client(hostname, 9999);

    MessageProtocol::get_io_service().run();
}
