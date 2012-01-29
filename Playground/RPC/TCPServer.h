#ifndef TCPSERVER_H
#define TCPSERVER_H


#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>


class TCPServer : boost::noncopyable
{
public:
    TCPServer();

    ~TCPServer();

    typedef boost::function<std::string(const std::string &)> Handler;

    void listen(short port, const Handler & handler);

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


#endif // TCPSERVER_H
