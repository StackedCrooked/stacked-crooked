#include "API.h"
#include "Core.h"
#include <iostream>


using namespace API;


int main (int argc, char * const argv[])
{
    APIServer server;
    APIInterface * interface = server.createInterface();
    
    interface->addStream(new APIStream);
    interface->addStream(new APIStream);
    APIStream * firstStream = interface->getStreamByIndex(0);
    firstStream->start();


    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
