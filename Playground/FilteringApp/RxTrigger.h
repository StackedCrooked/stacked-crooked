#pragma once


#include "Counter.h"
#include "Features.h"
#include "RxPacket.h"
#include "BPFFilter.h"
#include <vector>


struct RxTrigger
{
    RxTrigger(const std::string& filter);

    void process_one(RxPacket packet)
    {
        if (mBPFFilter.match(packet.data(), packet.size()))
        {
            mPackets++;
            mBytes += packet.size();
        }
    }

    void process_many(const std::vector<RxPacket>& packets)
    {
        for (const RxPacket& packet : packets)
        {
            process_one(packet);
        }
    }

    BPFFilter mBPFFilter;
    Counter mPackets = 0;
    Counter mBytes = 0;
};
