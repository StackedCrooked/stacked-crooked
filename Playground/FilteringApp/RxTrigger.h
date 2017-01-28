#pragma once


#include "RxPacket.h"


struct RxTrigger
{
    void process(RxPacket packet)
    {
        mPackets++;
        mBytes += packet.size();
    }

    uint64_t mPackets = 0;
    uint64_t mBytes = 0;
};
