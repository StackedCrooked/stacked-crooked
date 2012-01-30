#ifndef UDPCLIENT_H
#define UDPCLIENT_H


#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <map>
#include <string>
#include <vector>


class UDPServer : boost::noncopyable
{
public:
    // RequestHandler functor takes the request as a string and returns the reponse as a string.
    typedef boost::function<std::string(const std::string &)> RequestHandler;

    UDPServer(unsigned port, const RequestHandler & requestHandler);

    ~UDPServer();

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
