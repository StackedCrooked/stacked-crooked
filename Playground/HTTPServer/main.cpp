#include "http_server.h"
#include <ctime>


using namespace http;


struct TimeServer : http::Server
{
public:
    TimeServer(const std::string & host, unsigned short port) :
        http::Server(host, port)
    {

    }

private:
    std::string do_handle(const Request &)
    {
        return std::to_string(time(0));
    }
};


int main()
{
    TimeServer ts("localhost", 8080);
    ts.run();
}
