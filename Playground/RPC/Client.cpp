#include "Commands.h"
#include "Networking.h"
#include <iostream>
#include <utility>


using namespace boost::tuples;


void testSingle(RPCClient & client)
{
    std::cout << std::endl << "Testing Single Commands (sync)" << std::endl;
    RemoteStopwatch remoteStopwatch = client.send(CreateStopwatch("Stopwatch_01"));
    client.send(StartStopwatch(remoteStopwatch));
    std::cout << "Check: " << client.send(CheckStopwatch(remoteStopwatch)) << std::endl;
    std::cout << "Stop: " << client.send(StopStopwatch(remoteStopwatch)) << std::endl;
}


void testBatch(RPCClient & client)
{
    std::cout << std::endl << "Testing Batch Commands (sync)" << std::endl;
    std::vector<std::string> names;
    names.push_back("Stopwatch_01");
    names.push_back("Stopwatch_02");
    names.push_back("Stopwatch_03");

    // Create all stopwatches
    std::vector<RemoteStopwatch> stopwatches = client.send(Batch<CreateStopwatch>(names));

    // Start them
    client.send(Batch<StartStopwatch>(stopwatches));

    // Wait a second
    sleep(1);

    // Check their time
    std::vector<unsigned> el = client.send(Batch<CheckStopwatch>(stopwatches));
    std::cout << "Elapsed: ";
    for (std::size_t idx = 0; idx < el.size(); ++idx)
    {
        if (idx != 0)
        {
            std::cout << ", ";
        }
        std::cout << el[idx];
    }
    std::cout << std::endl;

    std::vector<unsigned> stopped = client.send(Batch<StopStopwatch>(stopwatches));
    std::cout << "Stopped " << stopped.size() << " stopwatches" << std::endl;
}


void run()
{
    RPCClient client("127.0.0.1", 9001);
    testSingle(client);
    testBatch(client);
}


int main()
{
    try
    {
        run();
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
