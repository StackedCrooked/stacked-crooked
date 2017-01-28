#pragma once


#include "RxPacket.h"
#include <vector>


struct Stack
{
    void add_to_queue(RxPacket packet)
    {
        mPackets.push_back(packet);
    }

    std::vector<RxPacket> mPackets;
};
