#pragma once


#include "PhysicalInterface.h"
#include "Array.h"


struct BBServer
{
    BBServer();

    PhysicalInterface& getPhysicalInterface(uint32_t i)
    {
        return mPhysicalInterfaces[i];
    }

    void run(const std::vector<RxPacket>& rxPackets, uint32_t num_repeats);

    Array<PhysicalInterface, 4> mPhysicalInterfaces;
};



