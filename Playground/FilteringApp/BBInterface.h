#pragma once


#include "BBPort.h"
#include "RxPacket.h"
#include <vector>


struct BBInterface
{
    BBInterface();
    BBPort& addPort(MACAddress localMAC);
    BBPort& getBBPort(uint32_t i) { return *mBBPorts[i]; }

    void pop(RxPacket packet)
    {
        // Also run any bpf filters.
        for (RxTrigger& rxTrigger : mRxTriggers)
        {
            rxTrigger.process(packet);
        }

        for (std::unique_ptr<BBPort>& port : mBBPorts)
        {
            BBPort& bbPort = *port;
            bbPort.pop_one(packet);
        }
    }

    void pop_later(RxPacket packet, std::vector<BBInterface*>& bb_interfaces)
    {
        if (mPacketBatch.empty())
        {
            bb_interfaces.push_back(this);
        }
        mPacketBatch.push_back(packet);
    }

    void pop_now();

    std::vector<std::unique_ptr<BBPort>> mBBPorts;
    std::vector<RxPacket> mPacketBatch;
    std::vector<RxTrigger> mRxTriggers;
};
