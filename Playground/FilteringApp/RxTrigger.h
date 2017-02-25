#pragma once


#include "Features.h"
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

    void process(const std::vector<RxPacket>& packets)
    {
        for (const RxPacket& packet : packets)
        {
            process(packet);
        }
    }

    BPFFilter mBPFFilter;
    Counter mPackets = 0;
    Counter mBytes = 0;
};
