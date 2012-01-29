#include "Commands.h"
#include "Networking.h"
#include "RemoteObjects.h"
#include <iostream>
#include <utility>


using namespace boost::tuples;


void testSingle()
{
    std::cout << std::endl << "Testing Single Commands (sync)" << std::endl;
    RemoteStopwatch remoteStopwatch = CreateStopwatch("Stopwatch_01").send();
    StartStopwatch(remoteStopwatch).send();
    std::cout << "Check: " << CheckStopwatch(remoteStopwatch).send() << std::endl;
    std::cout << "Stop: " << StopStopwatch(remoteStopwatch).send() << std::endl;
}


void testBatch()
{
    RemoteStopwatch rs = CreateStopwatch("Hello").send();
    (void)rs;

    std::vector<RemoteStopwatch> rs2 = Batch<CreateStopwatch>(std::vector<std::string>(2, "ABC")).send();
    std::cout << rs2.size() << std::endl;


    std::cout << std::endl << "Testing Batch Commands (sync)" << std::endl;
    std::vector<std::string> names;
    names.push_back("Stopwatch_01");
    names.push_back("Stopwatch_02");
    names.push_back("Stopwatch_03");

    // Create all stopwatches
    std::vector<RemoteStopwatch> stopwatches = Batch<CreateStopwatch>(names).send();

    // Start them
    Batch<StartStopwatch>(stopwatches).send();

    // Wait a second
    sleep(1);

    // Check their time
    std::vector<unsigned> el = Batch<CheckStopwatch>(stopwatches).send();
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

    std::vector<unsigned> stopped = Batch<StopStopwatch>(stopwatches).send();
    std::cout << "Stopped " << stopped.size() << " stopwatches" << std::endl;
}


void run()
{
    UDPClient client("127.0.0.1", 9001);
    Destination dest(boost::bind(&UDPClient::send, &client, _1));
    testSingle();
    testBatch();
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
