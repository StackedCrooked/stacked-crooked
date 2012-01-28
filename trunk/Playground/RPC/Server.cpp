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
        if (inName == Stopwatch_Create::Name())
        {
            std::string arg = deserialize<Stopwatch_Create::Arg>(inArg);
            mStopwatches.push_back(boost::make_shared<Stopwatch>(arg));
            Stopwatch_Create::Ret ret(RemotePtr(mStopwatches.back().get()), arg);
            return serialize(ret);
        }
//        else if (inName == Stopwatch_Start::Name())
//        {
//            RemoteStopwatch arg = deserialize<Stopwatch_Start::Arg>(inArg);
//            arg.getLocalObject().start();
//            return serialize(Void());
//        }
        else if (inName == Stopwatch_Elapsed::Name())
        {
            RemoteStopwatch arg = deserialize<Stopwatch_Elapsed::Arg>(inArg);
            Stopwatch & sw = arg.getLocalObject();
            return serialize(Stopwatch_Elapsed::Ret(sw.elapsedMs()));
        }
        else if (inName == Stopwatch_Stop::Name())
        {
            RemoteStopwatch arg = deserialize<Stopwatch_Stop::Arg>(inArg);
            Stopwatch & sw = arg.getLocalObject();
            sw.stop();
            return serialize(Stopwatch_Stop::Ret(sw.elapsedMs()));
        }
        else
        {
            throw std::runtime_error("Unknown RPC call: " + inName);
        }
    }

private:
    UDPServer mUDPServer;
};




int main()
{
    RPCServer rpcServer;
    (void)rpcServer;
}
