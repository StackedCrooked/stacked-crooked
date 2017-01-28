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
        for (BBPort& port : mBBPorts)
        {
            port.pop(packet);
        }

        // Also run any bpf filters.
        run_rx_triggers(packet);
    }

    void run_rx_triggers(RxPacket packet)
    {
        for (RxTrigger& rxTrigger : mRxTriggers)
        {
            rxTrigger.process(packet);
        }
    }

    std::vector<BBPort> mBBPorts;
    std::vector<RxTrigger> mRxTriggers;
};
