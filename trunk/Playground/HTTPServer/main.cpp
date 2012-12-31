#include "http_server.h"
#include <ctime>
#include <iostream>
#include <string>


using namespace http;


struct TimeServer : http::Server
{
public:
    TimeServer(const std::string & host, unsigned short port) :
        http::Server(host, port)
    {

    }

private:
    std::string do_handle(const Request & req)
    {
        for (auto c : req.payload)
        {
            std::cout << '[' << c << ']' << std::flush;
        }
        std::cout << std::endl;
        return std::to_string(time(0));
    }
};


int main()
{
    TimeServer ts("localhost", 8080);
    ts.run();
}
