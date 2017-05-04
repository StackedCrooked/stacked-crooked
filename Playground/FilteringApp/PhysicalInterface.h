#pragma once


#include "BBInterface.h"
#include "Array.h"
#include "RxPacket.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>



struct PhysicalInterface
{
    PhysicalInterface();

    void pop(const std::vector<RxPacket>& packets);

    BBInterface& getBBInterface(uint32_t i)
    {
        return mBBInterfaces[i];
    }

    std::vector<BBInterface> mBBInterfaces;
    std::vector<BBInterface*> mActiveInterfaces;
};
