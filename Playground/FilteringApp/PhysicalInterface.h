#pragma once


#include "BBInterface.h"
#include "RxPacket.h"
#include <array>
#include <cassert>
#include <vector>



struct PhysicalInterface
{
    void pop(RxPacket packet)
    {
        mBBInterface.pop(packet);
    }

    BBInterface& getBBInterface(uint32_t )
    {
        return mBBInterface;
    }

    BBInterface mBBInterface;
};
