#pragma once


#include "BBPort.h"
#include "RxPacket.h"
#include <vector>



struct PhysicalInterface;


struct BBInterface
{
    BBPort& addPort(PhysicalInterface& physicalInterface, MACAddress localMAC);
    BBPort& getBBPort(uint32_t i) { return mBBPorts[i]; }

    void pop(RxPacket packet)
    {
        for (BBPort& bbPort : mBBPorts)
        {
            bbPort.pop(packet);
        }
    }

    void pop_bpf(RxPacket packet)
    {

        for (RxTrigger& rxTrigger : mRxTriggers)
        {
            rxTrigger.process(packet);
        }
    }

    std::vector<BBPort> mBBPorts;
    std::vector<RxTrigger> mRxTriggers;
    uint64_t mPadding[2];
};
