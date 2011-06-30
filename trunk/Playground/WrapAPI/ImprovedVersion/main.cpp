#include "API.h"
#include "Core.h"
#include <iostream>


using namespace API;


int main (int argc, char * const argv[])
{
    Server * server = Server::Create();
    Interface * interface = server->createInterface();
    
    interface->addStream(Stream::Create());
    interface->addStream(Stream::Create());
    Stream * firstStream = interface->getStreamByIndex(0);
    firstStream->start();


    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
