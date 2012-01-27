#include "RPC/Commands.h"


using namespace RPC;
using boost::tuples::make_tuple;


void test()
{
    RemoteStopwatch res = CreateStopwatch(make_tuple(RemoteServer(), "")).run();
    std::cout << "Created stopwatch: " << res.remotePtr().value() << std::endl;
}
