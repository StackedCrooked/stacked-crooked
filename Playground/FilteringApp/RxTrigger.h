#pragma once


#include "RxPacket.h"


struct RxTrigger
{
    void process(RxPacket /*packet*/)
    {
        mCounter++;
    }

    uint64_t mCounter = 0;
};
