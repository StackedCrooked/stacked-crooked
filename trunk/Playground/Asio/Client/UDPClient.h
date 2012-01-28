#ifndef UDPCLIENT_H
#define UDPCLIENT_H


#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>


namespace Client {


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


} // namespace Client


#endif // UDPCLIENT_H
