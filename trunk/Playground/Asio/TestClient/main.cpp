#include "RPC/Commands.h"


using namespace RPC;
using boost::tuples::make_tuple;


void test()
{
    RemoteStopwatch remoteStopwatch = CreateStopwatch(make_tuple(RemoteServer(), "")).run();
    std::cout << "Created stopwatch: " << remoteStopwatch.remotePtr().value() << std::endl;

    bool ok = StartStopwatch(remoteStopwatch).run();
    std::cout << "Started stopwatch: " << ok << std::endl;

    unsigned time = StopStopwatch(remoteStopwatch).run();
    std::cout << "Stopped stopwatch. Time: " << time << std::endl;
}
