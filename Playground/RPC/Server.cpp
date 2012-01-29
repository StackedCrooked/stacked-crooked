#include "Networking.h"
#include "Commands.h"
#include "Stopwatch.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <utility>


using namespace boost::tuples;


typedef boost::shared_ptr<Stopwatch> StopwatchPtr;
typedef std::vector<StopwatchPtr> Stopwatches;
Stopwatches mStopwatches;


RPC_IMPLEMENT_CALL(RemoteStopwatch, Stopwatch_Create  , std::string     )
RPC_IMPLEMENT_CALL(Void,            Stopwatch_Start   , RemoteStopwatch )
RPC_IMPLEMENT_CALL(unsigned,        Stopwatch_Elapsed , RemoteStopwatch )
RPC_IMPLEMENT_CALL(unsigned,        Stopwatch_Stop    , RemoteStopwatch )
RPC_IMPLEMENT_CALL(Void,            Stopwatch_Destroy , RemoteStopwatch )

RemoteStopwatch Stopwatch_Create::Implement(const std::string &arg)
{
    mStopwatches.push_back(StopwatchPtr(new Stopwatch(arg)));
    return RemoteStopwatch(*mStopwatches.back());
}


Void Stopwatch_Start::Implement(const RemoteStopwatch & arg)
{
    arg.get()->start();
    return Void();
}


unsigned Stopwatch_Stop::Implement(const RemoteStopwatch & arg)
{
    return arg.get()->stop();
}


unsigned Stopwatch_Elapsed::Implement(const RemoteStopwatch &arg)
{
    return arg.get()->elapsedMs();
}


Void Stopwatch_Destroy::Implement(const RemoteStopwatch &)
{
    return Void(); // TODO: implement
}

struct RPCServer
{
    RPCServer(unsigned port = 9001) :
        mUDPServer(port)
    {
        mUDPServer.run(boost::bind(&RPCServer::processRequest, this, _1));
    }

    std::string processRequest(const std::string & inRequest)
    {

        NameAndArg nameAndArg = deserialize<NameAndArg>(inRequest);
        const std::string & name = nameAndArg.get<0>();
        const std::string & arg  = nameAndArg.get<1>();
        try
        {
            return serialize(RetOrError(true, processRequest(name, arg)));
        }
        catch (const std::exception & exc)
        {
            return serialize(RetOrError(false, exc.what()));
        }
    }

    std::string processRequest(const std::string & inName, const std::string & inArg)
    {
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
    RPCServer rpcServer;
    (void)rpcServer;
}
