#ifndef NETWORKING_H
#define NETWORKING_H


#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>


class UDPServer : boost::noncopyable
{
public:
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
    UDPClient(const std::string & inHost, unsigned inPort);

    ~UDPClient();

    std::string send(const std::string & inMessage);

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


#endif // NETWORKING_H
