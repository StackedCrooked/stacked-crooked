#pragma once


#include "PhysicalInterface.h"


struct BBServer
{
    BBServer();

    void run(std::vector<std::vector<uint8_t>>& batch, uint32_t num_packets);

    PhysicalInterface mPhysicalInterface;
};
