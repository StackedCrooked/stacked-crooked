#include "Networking.h"
#include <boost/bind.hpp>
#include <iostream>


std::string HandleRequest(const std::string & inRequest)
{
    return "Handled " + inRequest;
}


int main()
{
    UDPServer server(9001);
    server.run(boost::bind(&HandleRequest, _1));
}
