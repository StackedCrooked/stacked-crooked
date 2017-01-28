#pragma once


#include "PhysicalInterface.h"


struct BBServer
{
    BBServer();

    PhysicalInterface& getPhysicalInterface(uint32_t i)
    {
        return mPhysicalInterfaces[i];
    }

    void run(const std::vector<RxPacket>& rxPackets, uint32_t num_repeats);

    std::array<PhysicalInterface, 4> mPhysicalInterfaces;
};



