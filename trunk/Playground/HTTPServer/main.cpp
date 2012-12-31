#include "http_server.h"
#include <ctime>


struct TimeServer : HTTP::Server
{
public:
    TimeServer(const std::string & host, unsigned short port) :
        HTTP::Server(host, port)
    {

    }

private:
    std::string do_handle(const std::string &)
    {
        return std::to_string(time(0));
    }
};


int main()
{
    TimeServer ts("localhost", 8080);
}
