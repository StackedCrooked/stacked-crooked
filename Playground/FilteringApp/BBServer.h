#pragma once


#include "PhysicalInterface.h"


struct BBServer
{
    BBServer();

    PhysicalInterface& getPhysicalInterface(uint32_t i)
    {
        return mPhysicalInterfaces[i];
    }

    void run(const std::vector<std::vector<uint8_t> >& vec, uint32_t num_packets);

    std::array<PhysicalInterface, 4> mPhysicalInterfaces;
};



