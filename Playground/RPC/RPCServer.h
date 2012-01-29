#ifndef RPCSERVER_H
#define RPCSERVER_H


#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <map>
#include <string>


template<typename T>
std::string serialize(const T &);


template<typename T>
T deserialize(const std::string &);


class RPCServer : boost::noncopyable
{
public:
    RPCServer();

    ~RPCServer();

    // Start listening
    void listen(unsigned port);

    // Maps the function name to it's implementation.
    // Type erasure occurs here.
    template<typename Command>
    void registerCommand()
    {
        addHandler(Command::Name(), boost::bind(&RPCServer::process<Command>, _1));
    }

    template<typename Command>
    static std::string process(const std::string & serialized)
    {
        typedef typename Command::Arg Arg;
        typedef typename Command::Ret Ret;
        Arg arg = deserialize<Arg>(serialized);
        Ret ret = Command::execute(arg);
        return serialize(ret);
    }

private:
    typedef boost::function<std::string(const std::string &)> Handler;
    typedef std::map<std::string, Handler> Handlers;

    void addHandler(const std::string & inName, const Handler & inHandler);

    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


#endif // RPCSERVER_H
