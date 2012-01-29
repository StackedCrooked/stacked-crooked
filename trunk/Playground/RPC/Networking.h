#ifndef UDPCLIENT_H
#define UDPCLIENT_H


#include "Commands.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <map>
#include <string>
#include <vector>


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


struct RPCClient : private UDPClient
{
    RPCClient(const std::string & inURL, unsigned inPort) :
        UDPClient(inURL, inPort)
    {
    }

    template<typename Command>
    typename Command::Ret send(const Command & command)
    {
        std::string result = UDPClient::send(serialize(NameAndArg(Command::Name(), serialize(command.arg()))));
        RetOrError retOrError = deserialize<RetOrError>(result);
        if (retOrError.get_head())
        {
            return deserialize<typename Command::Ret>(retOrError.get<1>());
        }
        else
        {
            throw std::runtime_error("Server error: " + retOrError.get<1>());
        }
    }
};


#endif // UDPCLIENT_H
