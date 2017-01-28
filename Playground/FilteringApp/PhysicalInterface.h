#pragma once


#include "BBInterface.h"
#include "RxPacket.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>



struct PhysicalInterface
{
    void pop(RxPacket packet)
    {
        BBInterface& bbInterface = mBBInterfaces[packet.mVlanId];
        bbInterface.pop(packet);
    }

    BBInterface& getBBInterface(uint32_t i)
    {
        return mBBInterfaces[i];
    }

    std::array<BBInterface, 64> mBBInterfaces;
};
