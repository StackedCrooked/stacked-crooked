#pragma once


#include "BBPort.h"
#include "RxPacket.h"
#include <vector>


struct BBInterface
{
    BBPort& addPort(MACAddress localMAC);

    __attribute__((always_inline))
    void pop(RxPacket packet)
    {
        for (BBPort& port : mBBPorts)
        {
            port.pop(packet);
        }
    }

    BBPort& getBBPort(uint32_t i)
    {
        return mBBPorts[i];
    }

    std::vector<BBPort> mBBPorts;
};
