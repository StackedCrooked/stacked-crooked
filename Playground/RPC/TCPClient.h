#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>


class TCPClient : boost::noncopyable
{
public:
    TCPClient(const std::string & inURL, short port);

    ~TCPClient();

    std::string send(const std::string & inRequest);

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


#endif // TCPCLIENT_H
