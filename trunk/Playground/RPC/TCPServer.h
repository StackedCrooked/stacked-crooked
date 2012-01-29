#ifndef TCPSERVER_H
#define TCPSERVER_H


#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>


class TCPServer : boost::noncopyable
{
public:
    TCPServer(short port);

    ~TCPServer();

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


#endif // TCPSERVER_H
