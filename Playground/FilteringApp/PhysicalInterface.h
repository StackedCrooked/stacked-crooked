#pragma once


#include "BBInterface.h"
#include "RxPacket.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>



struct PhysicalInterface
{
    PhysicalInterface();

    __attribute__((always_inline))
    void pop(RxPacket packet)
    {
        BBInterface& bbInterface = mBBInterfaces[packet.vlanId()];
        bbInterface.pop(packet);
    }

    BBInterface& getBBInterface(uint32_t i)
    {
        return mBBInterfaces[i];
    }

    std::unique_ptr<BBInterface[]> mBBInterfaces;
};
