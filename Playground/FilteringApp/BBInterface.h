#pragma once


#include "BBPort.h"
#include "RxPacket.h"
#include <vector>


struct BBInterface
{
    BBPort& addPort(MACAddress localMAC);
    BBPort& getBBPort(uint32_t i) { return mBBPorts[i]; }

    void pop(RxPacket packet)
    {
        // Also run any bpf filters.
        for (RxTrigger& rxTrigger : mRxTriggers)
        {
            rxTrigger.process(packet);
        }

        for (BBPort& port : mBBPorts)
        {
            port.pop(packet);
        }
    }

    std::vector<BBPort> mBBPorts;
    std::vector<RxTrigger> mRxTriggers;
    uint64_t mPadding[2];
};
