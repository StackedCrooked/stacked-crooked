#pragma once


#include "PhysicalInterface.h"


struct BBServer
{
    BBServer();

    void run(const std::vector<std::vector<uint8_t> >& vec, uint32_t num_packets);

    PhysicalInterface& getPhysicalInterface(uint32_t i)
    {
        return mPhysicalInterfaces[i];
    }

    std::array<PhysicalInterface, 4> mPhysicalInterfaces;
};
