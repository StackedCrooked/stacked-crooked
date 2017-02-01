#pragma once


#include "BBInterface.h"
#include "RxPacket.h"
#include <boost/functional/hash.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>



struct PhysicalInterface
{
    void pop(const std::vector<RxPacket>& packets);

    BBPort& addPort(BBInterface& bbInterface, BBPort& bbPort);

    BBInterface& getBBInterface(uint32_t i)
    {
        return mBBInterfaces[i];
    }

    std::array<BBInterface, 64> mBBInterfaces;

    std::array<BBPort*, 128> mBBPortIndices;
};
