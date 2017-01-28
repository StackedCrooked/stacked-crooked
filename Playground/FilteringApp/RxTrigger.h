#pragma once


#include "RxPacket.h"
#include "BPFFilter.h"


struct RxTrigger
{
    RxTrigger(const std::string& filter);

    void process(RxPacket packet)
    {
        if (mBPFFilter.match(packet.data(), packet.size()))
        {
            mPackets++;
            mBytes += packet.size();
        }
    }

    BPFFilter mBPFFilter;
    uint64_t mPackets = 0;
    uint64_t mBytes = 0;
};
