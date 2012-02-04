#include "Core/RPCServer.h"
#include "Core/RemoteCall.h"
#include "Core/ConcurrentCall.h"
#include "Core/Test.h"
#include "Stopwatch.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <utility>


struct TestServer
{
public:
    static TestServer & Get()
    {
        static TestServer fTestServer;
        return fTestServer;
    }

    typedef boost::shared_ptr<Stopwatch> StopwatchPtr;
    typedef std::vector<StopwatchPtr> Stopwatches;

    Stopwatch & addStopwatch(const std::string & inName)
    {
        mStopwatches.push_back(StopwatchPtr(new Stopwatch(inName)));
        return *mStopwatches.back();
    }

    static long GetId(const StopwatchPtr & ptr)
    {
        return ptr->id();
    }

    void removeStopwatch(long id)
    {
        mStopwatches.erase(std::remove_if(mStopwatches.begin(), mStopwatches.end(), boost::bind(&TestServer::GetId, _1) == id));
    }

    Stopwatches getStopwatches()
    {
        return mStopwatches;
    }

    Stopwatch & getStopwatchById(long id)
    {
        for (std::size_t idx = 0; idx < mStopwatches.size(); ++idx)
        {
            Stopwatch & sw = *mStopwatches[idx];
            if (sw.id() == id)
            {
                return sw;
            }
        }
        throw std::runtime_error("Stopwatch with id " + boost::lexical_cast<std::string>(id) + " was not found.");
    }

private:
    Stopwatches mStopwatches;
};


namespace {

Stopwatch & getStopwatch(long inId)
{
    return TestServer::Get().getStopwatchById(inId);
}

}


RemoteStopwatch CreateStopwatch::execute(const std::string &arg)
{
    Stopwatch & sw = TestServer::Get().addStopwatch(arg);
    return RemoteStopwatch(sw.id(), sw.name());
}


RemoteStopwatches GetStopwatches::execute(const Void & )
{
    RemoteStopwatches result;
    TestServer::Stopwatches sw = TestServer::Get().getStopwatches();
    for (std::size_t idx = 0; idx < sw.size(); ++idx)
    {
        Stopwatch & s = *sw[idx];
        result.push_back(RemoteStopwatch(s.id(), s.name()));
    }
    return result;
}


Void StartStopwatch::execute(const RemoteStopwatch & arg)
{
    getStopwatch(arg.id()).start();
    return Void();
}


unsigned StopStopwatch::execute(const RemoteStopwatch & arg)
{
    return getStopwatch(arg.id()).stop();
}


unsigned CheckStopwatch::execute(const RemoteStopwatch &arg)
{
    return getStopwatch(arg.id()).elapsedMs();
}


Void DestroyStopwatch::execute(const RemoteStopwatch & arg)
{
    TestServer::Get().removeStopwatch(arg.id());
    return Void();
}


template<typename C>
void Register()
{
    RPCServer::registerCommand<C>();
}


int main()
{
    try
    {
        RPCServer server;
        const unsigned cPort = 9001;
        std::cout << "Listening to port " << cPort << std::endl;
        server.listen(cPort);
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
    }
}
