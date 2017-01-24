#pragma once


#include "BBPort.h"
#include "RxPacket.h"
#include <vector>


struct BBInterface
{
    BBPort& addPort(MACAddress localMAC);

    void pop(RxPacket packet)
    {
        for (BBPort& port : mBBPorts)
        {
            port.pop(packet);
        }
    }

    void pop_many(const RxPacket* packet_ptr, uint32_t length)
    {
        for (BBPort& port : mBBPorts)
        {
            port.pop_many(packet_ptr, length);
        }
    }

    std::vector<BBPort> mBBPorts;
};
