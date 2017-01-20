#pragma once


#include "PhysicalInterface.h"


struct BBServer
{
    BBServer();

    void run(const std::vector<std::vector<uint8_t> >& vec, uint32_t num_packets);

    PhysicalInterface mPhysicalInterface;
};
