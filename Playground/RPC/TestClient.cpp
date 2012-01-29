#include "RemoteCall.h"
#include "RemoteObjects.h"
#include "TCPClient.h"
#include <iostream>
#include <sstream>
#include <utility>


using namespace boost::tuples;


struct TestClient
{
    void testSingle()
    {
        std::cout << std::endl << "Testing Single Commands (sync)" << std::endl;
        RemoteStopwatch remoteStopwatch = CreateStopwatch("Stopwatch_01").send();
        StartStopwatch(remoteStopwatch).send();
        std::cout << "Check: " << CheckStopwatch(remoteStopwatch).send() << std::endl;
        std::cout << "Stop: " << StopStopwatch(remoteStopwatch).send() << std::endl;
        std::cout << std::endl;
    }

    void testBatch()
    {
        std::vector<std::string> names;
        for (std::size_t idx = 0; idx < 100; ++idx)
        {
            std::stringstream ss;
            ss << "Stopwatch_" << std::setw(3) << idx;
            names.push_back(ss.str());
        }

        RemoteStopwatches rs = Batch<CreateStopwatch>(names).send();
        std::cout << "Created " << rs.size() << " remote Stopwatches." << std::endl;

        Batch<StartStopwatch>(rs).send();
        std::cout << "Started " << rs.size() << " stopwatches." << std::endl;

        for (int i = 0; i < 10; ++i)
        {
            std::cout << "Updated times:" << std::endl;
            std::vector<unsigned> times = Batch<CheckStopwatch>(rs).send();
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

        std::vector<unsigned> stopTimes = Batch<StopStopwatch>(rs).send();
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
        TCPClient client("127.0.0.1", 9001);
        Redirector dest(boost::bind(&TCPClient::send, &client, _1));
        testSingle();
        testBatch();
    }

};


int main()
{
    try
    {
        TestClient tc;
        tc.run();
        return 0;
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
        return 1;
    }
}
