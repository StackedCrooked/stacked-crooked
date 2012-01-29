#include "Networking.h"
#include "Commands.h"
#include "Stopwatch.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <utility>


using namespace boost::tuples;


#define TRACE std::cout << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << std::endl;


typedef boost::shared_ptr<Stopwatch> StopwatchPtr;
typedef std::vector<StopwatchPtr> Stopwatches;
Stopwatches mStopwatches;


RemoteStopwatch CreateServer::Implement(const std::string &arg)
{
    TRACE
    mStopwatches.push_back(StopwatchPtr(new Stopwatch(arg)));
    return RemoteStopwatch(*mStopwatches.back());
}


Void StartStopwatch::Implement(const RemoteStopwatch & arg)
{
    TRACE
    arg.get()->start();
    return Void();
}


unsigned StopStopwatch::Implement(const RemoteStopwatch & arg)
{
    TRACE
    return arg.get()->stop();
}


unsigned CheckStopwatch::Implement(const RemoteStopwatch &arg)
{
    TRACE
    return arg.get()->elapsedMs();
}


Void DestroyStopwatch::Implement(const RemoteStopwatch &)
{
    TRACE
    return Void(); // TODO: implement
}

struct RPCServer
{
    RPCServer(unsigned port = 9001) :
        mUDPServer(port)
    {
        std::cout << "Listening to port " << port << std::endl;
        mUDPServer.run(boost::bind(&RPCServer::processRequest, this, _1));
    }

    std::string processRequest(const std::string & inRequest)
    {
        std::cout << "\n*** Begin Request:\n" << inRequest << std::endl;
        NameAndArg nameAndArg = deserialize<NameAndArg>(inRequest);
        const std::string & name = nameAndArg.get<0>();
        const std::string & arg  = nameAndArg.get<1>();
        std::cout << "Name: " << name << ", Arg: " << arg << std::endl;

        std::string result;
        try
        {
            result = serialize(RetOrError(true, processRequest(name, arg)));
        }
        catch (const std::exception & exc)
        {
            result = serialize(RetOrError(false, exc.what()));
        }
        std::cout << "*** End Request" << std::endl;
        return result;
    }

    std::string processRequest(const std::string & inName, const std::string & inArg)
    {
        std::cout << "Finding callback for: " << inName << std::endl;
        Runners::iterator it = GetRunners().find(inName);
        if (it == GetRunners().end())
        {
            throw std::runtime_error("Command not registered: " + inName);
        }
        Runner & runner = it->second;
        return runner(inArg);
    }

private:
    UDPServer mUDPServer;
};


int main()
{
    try
    {
        RPCServer rpcServer;
        (void)rpcServer;
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
    }
}
