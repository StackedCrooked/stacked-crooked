#pragma once


#include "BBPort.h"


#if 0
#include <boost/container/static_vector.hpp>
using BBPorts = boost::container::static_vector<BBPort, 2>;
#else
#include <vector>
using BBPorts = std::vector<BBPort>;
#endif



struct BBInterface
{
    BBPort& addPort(MACAddress localMAC)
    {
        mBBPorts.emplace_back(localMAC);
        return mBBPorts.back();
    }

    void pop(const uint8_t* data, uint32_t length)
    {
        for (BBPort& bbPort : mBBPorts)
        {
            bbPort.pop(data, length);
        }
    }

    BBPorts mBBPorts;
};
