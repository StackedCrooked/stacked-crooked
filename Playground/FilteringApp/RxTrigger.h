#pragma once


#include "RxPacket.h"


struct RxTrigger
{
    void process(RxPacket packet)
    {
        if (packet[0] == packet[1])
        {
            mPackets++;
            mBytes += packet.size();
        }
    }

    uint64_t mPackets = 0;
    uint64_t mBytes = 0;
};
