#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H


#include <memory>
#include <functional>
#include <string>
#include <vector>


namespace http {


struct Header
{
    std::string name;
    std::string value;
};


struct Request
{
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector<Header> headers;
};


class Server
{
public:
    Server(const std::string & host, unsigned short port);

    ~Server();

    void run();

private:
    virtual std::string do_handle(const Request & req) = 0;

    struct impl;
    std::unique_ptr<impl> impl_;
};


} // namespace HTTP


#endif // HTTP_SERVER_H
