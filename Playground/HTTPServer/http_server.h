#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H


#include <memory>
#include <functional>
#include <string>


namespace HTTP {


class Server
{
public:
    Server(const std::string & host, unsigned short port = 80);

    ~Server();

private:
    std::string handle(const std::string & req);

    virtual std::string do_handle(const std::string & req) = 0;

    struct impl;
    std::unique_ptr<impl> impl_;
};


} // namespace HTTP


#endif // HTTP_SERVER_H
