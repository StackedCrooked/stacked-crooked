#pragma once


#include "BBPort.h"
#include <vector>


struct BBInterface
{
    BBPort& addPort(MACAddress localMAC);

    void pop(const uint8_t* data, uint32_t length)
    {
        for (BBPort& bbPort : mBBPorts)
        {
            bbPort.pop(data, length);
        }
    }

    std::vector<BBPort> mBBPorts;
};
