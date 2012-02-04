#include "Core/Asio.h"
#include "Core/RemoteCall.h"
#include "Core/ConcurrentCall.h"
#include "Core/RemoteObjects.h"
#include "Core/Test.h"
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
            ss << "S" << std::setw(3) << idx;
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


    void testCombined()
    {
        std::cout << "Testing combined calls" << std::endl;

        std::cout << "First we create a stopwatch." << std::endl;
        RemoteStopwatch s = CreateStopwatch("s1").send();

        std::cout << "Concurrent call..." << std::endl;
        std::cout << "Now we request both StartStopwatch and StopStopwatch." << std::endl;
        boost::tuples::tuple<Void, unsigned> result = Combine(StartStopwatch(s), StopStopwatch(s));
        std::cout << "Result: Void and " << result.get<1>() << std::endl;
    }


    void testBatchAndCombined()
    {
        std::vector<std::string> names;
        for (std::size_t idx = 0; idx < 100; ++idx)
        {
            std::stringstream ss;
            ss << "S" << std::setw(3) << idx;
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
        UDPClient client("127.0.0.1", 9001);
        Redirector dest(boost::bind(&UDPClient::send, &client, _1));
        //testSingle();
        //testBatch();
        testCombined();
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
