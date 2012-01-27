#include "Server/RequestHandler.h"
#include <boost/lexical_cast.hpp>


static const unsigned cDefaultPort = 9001;


int main (int argc, char *argv[])
{
    std::vector<std::string> args(argv, argv + argc);
    unsigned port = args.size() <= 1 ? cDefaultPort : boost::lexical_cast<unsigned>(args[1]);

    Server::RequestHandler requestHandler(port);
    requestHandler.start();
    return 0;
}
