#pragma once


#include "BBInterface.h"
#include "RxPacket.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>



struct PhysicalInterface
{
    void pop(const std::vector<RxPacket>& packets);

    BBInterface& getBBInterface(uint32_t i)
    {
        return mBBInterfaces[i];
    }

    std::array<BBInterface, 64> mBBInterfaces;
};
