#pragma once


#include "BBPort.h"
#include "RxPacket.h"
#include <vector>


struct BBInterface
{
    BBInterface();
    BBPort& addPort(MACAddress localMAC);
    BBPort& getBBPort(uint32_t i) { return mBBPorts[i]; }

    void pop(RxPacket packet)
    {
        for (RxTrigger& rxTrigger : mRxTriggers)
        {
            rxTrigger.process(packet);
        }

        for (BBPort& port : mBBPorts)
        {
            port.pop_one(packet);
        }
    }

    void pop_later(RxPacket packet, std::vector<BBInterface*>& bb_interfaces)
    {
        if (mPackets.empty())
        {
            bb_interfaces.push_back(this);
        }
        mPackets.push_back(packet);
    }

    void pop_now()
    {
        for (BBPort& port : mBBPorts)
        {
            port.pop_many(mPackets.data(), mPackets.size());
        }

        for (RxTrigger& rxTrigger : mRxTriggers)
        {
            rxTrigger.process(mPackets);
        }

        mPackets.clear();
    }

    std::vector<BBPort> mBBPorts;
    std::vector<RxPacket> mPackets;
    std::vector<RxTrigger> mRxTriggers;
};
