#include "Commands.h"
#include "Networking.h"
#include <iostream>
#include <utility>


using namespace boost::tuples;


template<typename Command>
typename Command::Ret send(UDPClient & client, const Command & command)
{
    std::cout << Command::CommandName() << ": ";
    std::string result = client.send(serialize(NameAndArg(Command::CommandName(), serialize(command.arg()))));
    std::cout << result << std::endl;
    return deserialize<typename Command::Ret>(result);
}


int main()
{
    UDPClient client("127.0.0.1", 9001);
    RemoteStopwatch s1 = send(client, CreateStopwatch("Stopwatch_01"));
    RemoteStopwatch s2 = send(client, CreateStopwatch("Stopwatch_02"));
    RemoteStopwatch s3 = send(client, CreateStopwatch("Stopwatch_03"));

    std::cout << "s1: " << s1.name() << std::endl;
    std::cout << "s2: " << s2.name() << std::endl;
    std::cout << "s3: " << s3.name() << std::endl;

    (void)s1;
    (void)s2;
    (void)s3;

    send(client, StartStopwatch(s1));
//    send(client, StartStopwatch("Stopwatch_02"));
//    send(client, StartStopwatch("Stopwatch_03"));

//    send(client, StopStopwatch("Stopwatch_01"));
//    send(client, StopStopwatch("Stopwatch_02"));
//    send(client, StopStopwatch("Stopwatch_03"));
}
