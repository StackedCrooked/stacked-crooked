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
    void pop(const std::vector<RxPacket>& packets);

    BBInterface& getBBInterface(uint32_t i)
    {
        return mBBInterfaces[i];
    }

    Array<BBInterface, 63> mBBInterfaces;
    std::vector<BBInterface*> mActiveInterfaces;
    uint8_t mPadding[104];
};
