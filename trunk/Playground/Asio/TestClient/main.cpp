#include "RPC/Commands.h"


using namespace RPC;


void test()
{
    CreateStopwatch(RemoteServer(), "").run();
}
