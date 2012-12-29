#ifndef NETWORKING_H
#define NETWORKING_H


#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/strong_typedef.hpp>
#include <string>


BOOST_STRONG_TYPEDEF(std::string, Request)
BOOST_STRONG_TYPEDEF(std::string, Response)



class UDPServer : boost::noncopyable
{
public:
    typedef boost::function<Response(const Request &)> RequestHandler;

    UDPServer(unsigned local_port, const RequestHandler & requestHandler);

    ~UDPServer();

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


class UDPClient : boost::noncopyable
{
public:
    UDPClient(const std::string & remote_host, unsigned remote_port);

    ~UDPClient();

    std::string send(const std::string &);

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


class UDPReceiver : boost::noncopyable
{
public:
    typedef boost::function<bool(const std::string &)> RequestHandler;

    UDPReceiver(unsigned port, const RequestHandler & handler);

    ~UDPReceiver();

    void waitForAll();

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


class UDPSender : boost::noncopyable
{
public:
    UDPSender(const std::string & inHost, unsigned inPort);

    ~UDPSender();

    void send(const std::string & inMessage);

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


#endif // NETWORKING_H
