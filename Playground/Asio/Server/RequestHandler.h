#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H


#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>


namespace Server {


class Client
{
public:
    Client(const std::string & inIP) :
        mIP(inIP)
    {
    }

    const std::string & ip() const { return mIP; }

private:
    std::string mIP;
};


class RequestHandler
{
public:
    RequestHandler(unsigned inPort) :
        mPort(inPort)
    {
    }

    void start()
    {

    }

    void connect(const std::string & inIP)
    {
        mClients.push_back(boost::make_shared<Client>(inIP));
    }

private:
    unsigned mPort;
    typedef boost::shared_ptr<Client> ClientPtr;
    std::vector<ClientPtr> mClients;
};


} // namespace Server


#endif // SERVER_SERVER_H
