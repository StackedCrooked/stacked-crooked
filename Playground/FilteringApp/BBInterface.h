#pragma once


#include "BBPort.h"
#include "RxPacket.h"
#include <vector>


struct BBInterface
{
    BBPort& addPort(MACAddress localMAC);

    void pop(RxPacket packet)
    {
        mBBPorts[packet.mBBInterfaceId].pop(packet);
    }

    std::vector<BBPort> mBBPorts;
};
