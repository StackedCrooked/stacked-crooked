#ifndef TEST_H
#define TEST_H


#include "RemoteCall.h"


RPC_CALL(CreateStopwatch,  RemoteStopwatch(std::string) )
RPC_CALL(GetStopwatches,   RemoteStopwatches(Void)      )
RPC_CALL(StartStopwatch,   Void(RemoteStopwatch)        )
RPC_CALL(CheckStopwatch,   unsigned(RemoteStopwatch)    )
RPC_CALL(StopStopwatch,    unsigned(RemoteStopwatch)    )
RPC_CALL(DestroyStopwatch, Void(RemoteStopwatch)        )


#endif // TEST_H
