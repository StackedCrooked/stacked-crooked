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
        std::cout << "HandleRequest: name: " << name << std::endl;
        if (name == CreateStopwatch::CommandName())
        {
            std::string arg = deserialize<CreateStopwatch::Arg>(nameAndArg.get<1>());
            mStopwatches.push_back(boost::make_shared<Stopwatch>(arg));
            CreateStopwatch::Ret ret(RemotePtr(mStopwatches.back().get()), arg);
            return serialize(ret);
        }
        else if (name == StartStopwatch::CommandName())
        {
            RemoteStopwatch arg = deserialize<StartStopwatch::Arg>(nameAndArg.get<1>());
            arg.remotePtr().cast<Stopwatch>().start();
            return serialize(Void());
        }
        else if (name == StopStopwatch::CommandName())
        {
            RemoteStopwatch arg = deserialize<StopStopwatch::Arg>(nameAndArg.get<1>());
            return serialize(Stopwatch::Ret(arg.remotePtr().cast<Stopwatch>()));
        }
        return "Unknown command: " + inRequest;
    }

    UDPServer mUDPServer;
};




int main()
{
    RPCServer rpcServer;
    (void)rpcServer;
}
