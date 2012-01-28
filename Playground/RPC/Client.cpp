#include "Commands.h"
#include "Networking.h"
#include <iostream>
#include <utility>


using namespace boost::tuples;


template<typename Command>
typename Command::Ret send(UDPClient & client, const Command & command)
{
    std::cout << Command::Name() << std::endl;
    std::string result = client.send(serialize(NameAndArg(Command::Name(), serialize(command.arg()))));
    return deserialize<typename Command::Ret>(result);
}


int main()
{
    UDPClient client("127.0.0.1", 9001);
    RemoteStopwatch s1 = send(client, Stopwatch_Create("Stopwatch_01"));
    std::cout << "s1: " << s1.name() << std::endl;

    RemoteStopwatch s2 = send(client, Stopwatch_Create("Stopwatch_02"));
    std::cout << "s2: " << s2.name() << std::endl;

    RemoteStopwatch s3 = send(client, Stopwatch_Create("Stopwatch_03"));
    std::cout << "s3: " << s3.name() << std::endl;

    (void)s1;
    (void)s2;
    (void)s3;

    send(client, Stopwatch_Start(s1));
    send(client, Stopwatch_Stop(s1));
}
