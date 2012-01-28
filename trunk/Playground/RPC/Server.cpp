#include "Networking.h"
#include "Commands.h"
#include "Stopwatch.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>


using namespace boost::tuples;


typedef boost::shared_ptr<Stopwatch> StopwatchPtr;
typedef std::vector<StopwatchPtr> Stopwatches;
Stopwatches mStopwatches;


std::string GetName(StopwatchPtr ptr)
{
    return ptr->name();
}


//RPC_IMPLEMENT_CALL(CreateP)


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
        return processRequest(name, arg);
    }

    std::string process(const std::string & inName, const std::string & inArg)
    {
        if (name == Stopwatch_Create::Name())
        {
            std::string arg = deserialize<Stopwatch_Create::Arg>(nameAndArg.get<1>());
            mStopwatches.push_back(boost::make_shared<Stopwatch>(arg));
            Stopwatch_Create::Ret ret(RemotePtr(mStopwatches.back().get()), arg);
            return serialize(ret);
        }
        else if (name == Stopwatch_Start::Name())
        {
            RemoteStopwatch arg = deserialize<Stopwatch_Start::Arg>(nameAndArg.get<1>());
            arg.getLocalObject().start();
            return serialize(Void());
        }
        else if (name == Stopwatch_Elapsed::Name())
        {
            RemoteStopwatch arg = deserialize<Stopwatch_Elapsed::Arg>(nameAndArg.get<1>());
            Stopwatch & sw = arg.getLocalObject();
            return serialize(Stopwatch_Elapsed::Ret(sw.elapsedMs()));
        }
        else if (name == Stopwatch_Stop::Name())
        {
            RemoteStopwatch arg = deserialize<Stopwatch_Stop::Arg>(nameAndArg.get<1>());
            Stopwatch & sw = arg.getLocalObject();
            sw.stop();
            return serialize(Stopwatch_Stop::Ret(sw.elapsedMs()));
        }
        return "Unknown command: " + inRequest;
    }

private:
    UDPServer mUDPServer;
};




int main()
{
    RPCServer rpcServer;
    (void)rpcServer;
}
