#pragma once


#include "Array.h"
#include "RxPacket.h"
#include "BPFFilter.h"
#include <vector>


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

    void process(const Vector<RxPacket, 32>& packets)
    {
        for (const RxPacket& packet : packets)
        {
            process(packet);
        }
    }

    BPFFilter mBPFFilter;
    uint64_t mPackets = 0;
    uint64_t mBytes = 0;
};
