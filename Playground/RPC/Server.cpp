#include "Networking.h"
#include "Commands.h"
#include "Stopwatch.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <utility>


typedef std::set<long> RemotePointers;
RemotePointers mRemotePointers;

typedef boost::shared_ptr<Stopwatch> StopwatchPtr;
typedef std::vector<StopwatchPtr> Stopwatches;
Stopwatches mStopwatches;



template<typename T>
T & get(const RemoteObject<T> & remote)
{
    RemotePtr ptr = remote.mRemotePtr;
    RemotePointers::const_iterator it = mRemotePointers.find(ptr.mValue);
    if (it == mRemotePointers.end())
    {
        throw std::runtime_error("Remote object no longer exists.");
    }
    long value = *it;
    return *reinterpret_cast<T*>(value);
}


RemoteStopwatch Stopwatch_Create::Implement(const std::string &arg)
{
    mStopwatches.push_back(StopwatchPtr(new Stopwatch(arg)));
    return RemoteStopwatch(*mStopwatches.back());
}


Void Stopwatch_Start::Implement(const RemoteStopwatch & arg)
{
    get(arg).start();
    return Void();
}


unsigned Stopwatch_Stop::Implement(const RemoteStopwatch & arg)
{
    return get(arg).stop();
}


Void Stopwatch_Restart::Implement(const RemoteStopwatch &arg)
{
    get(arg).restart();
    return Void();
}


unsigned Stopwatch_Elapsed::Implement(const RemoteStopwatch &arg)
{
    return get(arg).elapsedMs();
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
