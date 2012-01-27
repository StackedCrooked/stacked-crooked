#include "RPC/Commands.h"
#include <boost/serialization/string.hpp>


using namespace RPC;
using boost::tuples::make_tuple;


void print(const RemoteServer & server)
{
    std::cout << "server remote ptr: " << server.remotePtr().value() << std::endl;
    std::cout << "server url: " << server.url() << std::endl;
}


void test()
{
    RemoteServer remoteServer(RemotePtr(10), "remote-server.com");
    print(remoteServer);

    std::string serialized = serialize(remoteServer);
    std::cout << "serialized: " << serialized << std::endl;

    RemoteServer deserialized = deserialize<RemoteServer>(serialized);
    print(deserialized);

//    std::cout << serialize(make_tuple(RemoteServer(), "stopwatch1")) << std::endl;

//    RemoteStopwatch remoteStopwatch = CreateStopwatch(make_tuple(RemoteServer(), "")).run();
//    std::cout << "Created stopwatch: " << remoteStopwatch.remotePtr().value() << std::endl;

//    bool ok = StartStopwatch(remoteStopwatch).run();
//    std::cout << "Started stopwatch: " << ok << std::endl;

//    unsigned time = StopStopwatch(remoteStopwatch).run();
//    std::cout << "Stopped stopwatch. Time: " << time << std::endl;

}


int main()
{
    test();
}
