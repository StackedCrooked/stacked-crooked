#include "RPC.h"
#include "RemoteObjects.h"
#include "Stopwatch.h"
#include <boost/lexical_cast.hpp>



RPC_CALL(CreateStopwatch,  RemoteStopwatch(std::string) )
RPC_CALL(GetStopwatches,   RemoteStopwatches(Void)      )
RPC_CALL(StartStopwatch,   Void(RemoteStopwatch)        )
RPC_CALL(CheckStopwatch,   unsigned(RemoteStopwatch)    )
RPC_CALL(StopStopwatch,    unsigned(RemoteStopwatch)    )
RPC_CALL(DestroyStopwatch, Void(RemoteStopwatch)        )


#ifdef RPC_SERVER


struct TestServer
{
public:
    static TestServer & Instance()
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


static Stopwatch & getStopwatch(long inId)
{
    return TestServer::Instance().getStopwatchById(inId);
}


RemoteStopwatch CreateStopwatch::execute(RPCServer &, const std::string &arg)
{
    Stopwatch & sw = TestServer::Instance().addStopwatch(arg);
    return RemoteStopwatch(sw.id(), sw.name());
}


RemoteStopwatches GetStopwatches::execute(RPCServer &, const Void & )
{
    RemoteStopwatches result;
    TestServer::Stopwatches sw = TestServer::Instance().getStopwatches();
    for (std::size_t idx = 0; idx < sw.size(); ++idx)
    {
        Stopwatch & s = *sw[idx];
        result.push_back(RemoteStopwatch(s.id(), s.name()));
    }
    return result;
}


Void StartStopwatch::execute(RPCServer &, const RemoteStopwatch & arg)
{
    getStopwatch(arg.id()).start();
    return Void();
}


unsigned StopStopwatch::execute(RPCServer &, const RemoteStopwatch & arg)
{
    return getStopwatch(arg.id()).stop();
}


unsigned CheckStopwatch::execute(RPCServer &, const RemoteStopwatch &arg)
{
    return getStopwatch(arg.id()).elapsedMs();
}


Void DestroyStopwatch::execute(RPCServer &, const RemoteStopwatch & arg)
{
    TestServer::Instance().removeStopwatch(arg.id());
    return Void();
}


void runServer(short port)
{
    std::cout << "Listening to port " << port << std::endl;
    RPCServer::Instance().listen(port);
}


#endif // RPC_SERVER


//
// Client code
//
#ifdef RPC_CLIENT


struct TestClient
{
    TestClient(const std::string & host, short port) :
        client(host, port)
    {
    }

    void testSingle()
    {
        std::cout << std::endl << "Testing Single Commands (sync)" << std::endl;
        RemoteStopwatch remoteStopwatch = client.send(CreateStopwatch("Stopwatch_01"));
        client.send(StartStopwatch(remoteStopwatch));
        std::cout << "Check: " << client.send(CheckStopwatch(remoteStopwatch)) << std::endl;
        std::cout << "Stop: " << client.send(StopStopwatch(remoteStopwatch)) << std::endl;
        std::cout << std::endl;
    }


    void testBatch()
    {
        std::vector<std::string> names;
        for (std::size_t idx = 0; idx < 100; ++idx)
        {
            std::stringstream ss;
            ss << "S" << std::setw(3) << idx;
            names.push_back(ss.str());
        }

        RemoteStopwatches rs = client.send(Batch<CreateStopwatch>(names));
        std::cout << "Created " << rs.size() << " remote Stopwatches." << std::endl;

        client.send(Batch<StartStopwatch>(rs));
        std::cout << "Started " << rs.size() << " stopwatches." << std::endl;

        for (int i = 0; i < 10; ++i)
        {
            std::cout << "Updated times:" << std::endl;
            std::vector<unsigned> times = client.send(Batch<CheckStopwatch>(rs));
            for (std::size_t idx = 0; idx < times.size(); ++idx)
            {
                if (idx != 0)
                {
                    std::cout << ", ";
                }
                std::cout << idx << ": " << times[idx];
            }
            std::cout << std::endl << std::endl;
            sleep(1);
        }

        std::vector<unsigned> stopTimes = client.send(Batch<StopStopwatch>(rs));
        std::cout << "Stopped " << stopTimes.size() << " stopwatches. Times: " << std::endl;

        for (std::size_t idx = 0; idx < stopTimes.size(); ++idx)
        {
            if (idx != 0)
            {
                std::cout << ", ";
            }
            std::cout << idx << ": " << stopTimes[idx];
        }
        std::cout << std::endl;
    }

    void run()
    {
        testSingle();
        testBatch();
    }

private:
    RPCClient client;
};


void runClient(const std::string & host, short port)
{
    TestClient client(host, port);
    client.run();
}


#endif // RPC_CLIENT


int main()
{
    short port = 9001;
#if RPC_SERVER
    runServer(port);
#else
    runClient("127.0.0.1", port);
#endif
}
