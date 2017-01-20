#pragma once


#include "BBPort.h"
#include "RxPacket.h"
#include <vector>


struct BBInterface
{
    BBPort& addPort(MACAddress localMAC);

    void pop(RxPacket packet)
    {
        for (BBPort& bbPort : mBBPorts)
        {
            bbPort.pop(packet);
        }
    }

    std::vector<BBPort> mBBPorts;
};
