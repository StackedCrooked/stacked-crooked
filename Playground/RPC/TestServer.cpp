#include "RPCServer.h"
#include "RemoteCall.h"
#include "Stopwatch.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <utility>


RPCServer & GetRPCServer()
{
    static RPCServer fInstance;
    return fInstance;
}


using namespace boost::tuples;
typedef boost::shared_ptr<Stopwatch> StopwatchPtr;
typedef std::vector<StopwatchPtr> Stopwatches;
Stopwatches mStopwatches;


RemoteStopwatch CreateStopwatch::execute(const std::string &arg)
{
    TRACE
    mStopwatches.push_back(StopwatchPtr(new Stopwatch(arg)));
    return RemoteStopwatch(*mStopwatches.back());
}


Void StartStopwatch::execute(const RemoteStopwatch & arg)
{
    TRACE
    arg.get()->start();
    return Void();
}


unsigned StopStopwatch::execute(const RemoteStopwatch & arg)
{
    TRACE
    unsigned res = arg.get()->stop();
    std::cout << "Elapsed on stop: " << res << std::endl;
    return res;

}


unsigned CheckStopwatch::execute(const RemoteStopwatch &arg)
{
    TRACE
    std::cout << "Elapsed on check: " << arg.get()->elapsedMs() << std::endl;
    return arg.get()->elapsedMs();
}


Void DestroyStopwatch::execute(const RemoteStopwatch &)
{
    TRACE
    return Void(); // TODO: implement
}


template<typename C>
void Register()
{
    GetRPCServer().registerCommand<C>();
}


int main()
{
    try
    {
        const unsigned cPort = 9001;
        std::cout << "Listening to port " << cPort << std::endl;
        GetRPCServer().listen(cPort);
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
    }
}
