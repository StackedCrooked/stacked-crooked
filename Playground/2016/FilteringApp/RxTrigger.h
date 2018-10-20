#pragma once


#include "RxPacket.h"
#include "BPFFilter.h"
#include <vector>


struct RxTrigger
{
    RxTrigger(const std::string& filter);

    void process(const RxPacket& packet)
    {
        if (mBPFFilter.match(packet.data(), packet.size()))
        {
            mPackets++;
            mBytes += packet.size();
        }
    }

    void process(const std::vector<RxPacket>& packets)
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
