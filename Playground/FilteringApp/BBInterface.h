#pragma once


#include "Array.h"
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
            port->pop(packet);
        }
    }

    void pop_later(RxPacket packet, Vector<BBInterface*, 48>& bb_interfaces)
    {
        if (mPackets.empty())
        {
            bb_interfaces.push_back(this);
        }
        mPackets.push_back(packet);
    }

    void pop_now()
    {
        for (std::unique_ptr<BBPort>& port : mBBPorts)
        {
            port->pop_many(mPackets.data(), mPackets.size());
        }

        for (RxTrigger& rxTrigger : mRxTriggers)
        {
            rxTrigger.process(mPackets);
        }

        mPackets.clear();
    }

    std::vector<std::unique_ptr<BBPort>> mBBPorts;
    Vector<RxPacket, 32> mPackets;
    std::vector<RxTrigger> mRxTriggers;
};
