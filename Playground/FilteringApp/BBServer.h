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

    void prepare(const std::vector<std::vector<uint8_t> >& batch);

    std::array<PhysicalInterface, 4> mPhysicalInterfaces;
    std::vector<RxPacket> mRxPackets;
};



