#include "Commands.h"
#include "Networking.h"
#include <iostream>
#include <utility>


using namespace boost::tuples;


template<typename Command>
typename Command::Ret send(UDPClient & client, const Command & command)
{
    std::string result = client.send(serialize(NameAndArg(Command::Name(), serialize(command.arg()))));
    RetOrError retOrError = deserialize<RetOrError>(result);
    if (retOrError.get_head())
    {
        return deserialize<typename Command::Ret>(retOrError.get<1>());
    }
    else
    {
        throw std::runtime_error("Server error: " + retOrError.get<1>());
    }
}


void testChain(UDPClient & client)
{
    Void v = send(client, ChainedCommand<CreateStopwatch, StartStopwatch>("ChainedStopwatch"));
    (void)v;
}

void run()
{
    UDPClient client("127.0.0.1", 9001);

    testChain(client);

    RemoteStopwatch remoteStopwatch = send(client, CreateStopwatch("Stopwatch_01"));
    send(client, StartStopwatch(remoteStopwatch));
    sleep(1);
    std::cout << "Elapsed: " << send(client, StopStopwatch(remoteStopwatch)) << std::endl;

    std::cout << "Parallel testing" << std::endl;


    std::vector<std::string> names;
    names.push_back("a");
    names.push_back("b");
    names.push_back("c");

    std::vector<RemoteStopwatch> sw = send(client, ParallelCommand<CreateStopwatch>(names));
    std::cout << "Created " << sw.size() << " stopwatches." << std::endl;

    std::vector<Void> started = send(client, ParallelCommand<StartStopwatch>(sw));
    std::cout << "Started " << started.size() << " stopwatches" << std::endl;

    std::cout << "Sleep for 1 second..." << std::endl;

    std::vector<unsigned> el = send(client, ParallelCommand<CheckStopwatch>(sw));
    std::cout << "Checked: " << el.size() << " stopwatches" << std::endl;

    for (std::size_t idx = 0; idx < el.size(); ++idx)
    {
        std::cout << "Elapsed time: " << el[idx] << std::endl;
    }

    std::vector<unsigned> stopped = send(client, ParallelCommand<StopStopwatch>(sw));
    std::cout << "Stopped " << stopped.size() << " stopwatches" << std::endl;
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
