#ifndef UDPCLIENT_H
#define UDPCLIENT_H


#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>


class UDPServer
{
public:
    UDPServer(unsigned port);

    ~UDPServer();

    // Handler receives the request as a string and returns the reponse as a string.
    typedef boost::function<std::string(const std::string &)> RequestHandler;

    void run(const RequestHandler & requestHandler);

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


class UDPClient : boost::noncopyable
{
public:
    UDPClient(const std::string & inURL, unsigned inPort);

    ~UDPClient();

    std::string send(const std::string & inMessage);

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


#endif // UDPCLIENT_H
