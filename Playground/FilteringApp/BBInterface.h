#pragma once


#include "BBPort.h"
#include <vector>



struct BBInterface
{
    void addPort(MACAddress localMAC)
    {
        mBBPorts.emplace_back(localMAC);

    }

    void pop(const uint8_t* data, uint32_t length)
    {
        for (BBPort& bbPort : mBBPorts)
        {
            bbPort.pop(data, length);
        }
    }

    std::vector<BBPort> mBBPorts;
};
