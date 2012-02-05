#include "RPC.h"
#include "RemoteObjects.h"
#include "Stopwatch.h"
#include <boost/lexical_cast.hpp>


RPC_COMMAND(CreateStopwatch,  RemoteStopwatch(std::string) )
RPC_COMMAND(GetStopwatches,   RemoteStopwatches(Void)      )
RPC_COMMAND(StartStopwatch,   Void(RemoteStopwatch)        )
RPC_COMMAND(CheckStopwatch,   unsigned(RemoteStopwatch)    )
RPC_COMMAND(StopStopwatch,    unsigned(RemoteStopwatch)    )
RPC_COMMAND(DestroyStopwatch, Void(RemoteStopwatch)        )


#ifdef RPC_SERVER


struct ServerApplication
{
public:

    ServerApplication()
    {
        std::vector<std::string> commands = RPCServer::Instance().getRegisteredCommands();
        std::cout << "Registered commands:" << std::endl;
        for (std::size_t idx = 0; idx < commands.size(); ++idx)
        {
            std::cout << commands[idx] << std::endl;
        }
        std::cout << std::endl;
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
        mStopwatches.erase(std::remove_if(mStopwatches.begin(), mStopwatches.end(), boost::bind(&ServerApplication::GetId, _1) == id));
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


ServerApplication gServerApp;


static Stopwatch & getStopwatch(long inId)
{
    return gServerApp.getStopwatchById(inId);
}


RemoteStopwatch CreateStopwatch::execute(RPCServer &, const std::string &arg)
{
    Stopwatch & sw = gServerApp.addStopwatch(arg);
    return RemoteStopwatch(sw.id(), sw.name());
}


RemoteStopwatches GetStopwatches::execute(RPCServer &, const Void & )
{
    RemoteStopwatches result;
    ServerApplication::Stopwatches sw = gServerApp.getStopwatches();
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
    gServerApp.removeStopwatch(arg.id());
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


struct ClientApplication
{
    ClientApplication(const std::string & host, short port) :
        client(host, port)
    {
    }

    void testSingle()
    {
        std::cout << std::endl << "Testing Single Commands (sync)" << std::endl;
        RemoteStopwatch remoteStopwatch = client.send(CreateStopwatch("Stopwatch_01"));
        client.send(StartStopwatch(remoteStopwatch));
        sleep(1);
        std::cout << "Check: " << client.send(CheckStopwatch(remoteStopwatch)) << std::endl;
        sleep(1);
        std::cout << "Stop: " << client.send(StopStopwatch(remoteStopwatch)) << std::endl;
        std::cout << std::endl;
    }

#if 0
    void testForeach()
    {
        std::vector<std::string> names;
        for (std::size_t idx = 0; idx < 5; ++idx)
        {
            std::stringstream ss;
            ss << "Stopwatch_" << std::setfill('0') << std::setw(2) << idx;
            names.push_back(ss.str());
        }

        RemoteStopwatches rs = Foreach<CreateStopwatch>(client, names);
        std::cout << "Created " << rs.size() << " remote Stopwatches." << std::endl;

        Foreach<StartStopwatch>(client, rs);
        std::cout << "Started " << rs.size() << " stopwatches." << std::endl;

        for (int i = 0; i < 1; ++i)
        {
            std::cout << "Updated times:" << std::endl;
            std::vector<unsigned> times = Foreach<CheckStopwatch>(client, rs);
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

        std::vector<unsigned> stopTimes = Foreach<StopStopwatch>(client, rs);
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
#endif

    void run()
    {
        testSingle();
    }

private:
    RPCClient client;
};


void runClient(const std::string & host, short port)
{
    ClientApplication client(host, port);
    client.run();
}


#endif // RPC_CLIENT


void run()
{
    short port = 9001;
#if RPC_SERVER
    runServer(port);
#else
    runClient("127.0.0.1", port);
#endif
}


int main()
{
    try
    {
        run();
        return 0;
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
        return 1;
    }
}
