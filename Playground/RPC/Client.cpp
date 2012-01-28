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


void run()
{
    UDPClient client("127.0.0.1", 9001);


    RemoteStopwatch s1 = send(client, Stopwatch_Create("Stopwatch_01"));
    std::cout << "Created " << s1.name() << std::endl;

    RemoteStopwatch s2 = send(client, Stopwatch_Create("Stopwatch_02"));
    std::cout << "Created " << s2.name() << std::endl;

    send(client, Stopwatch_Start(s1));
    std::cout << "Started " << s1.name() << std::endl;

    send(client, Stopwatch_Start(s2));
    std::cout << "Started " << s2.name() << std::endl;

    std::cout << "Sleep for 1 second..." << std::endl;
    sleep(1);

    std::cout << "Progress for " << s1.name() << ": " << send(client, Stopwatch_Elapsed(s1)) << "ms" << std::endl;
    std::cout << "Progress for " << s2.name() << ": " << send(client, Stopwatch_Elapsed(s2)) << "ms" << std::endl;

    send(client, Stopwatch_Stop(s1));
    std::cout << "Stopped " << s1.name() << std::endl;

    send(client, Stopwatch_Stop(s2));
    std::cout << "Stopped " << s2.name() << std::endl;
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
