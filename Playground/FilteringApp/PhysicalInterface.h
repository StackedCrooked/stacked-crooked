#pragma once


#include "BBInterface.h"
#include <array>
#include <cassert>
#include <vector>



struct PhysicalInterface
{
    void pop(const uint8_t* data, uint32_t length, uint32_t interfaceId)
    {
        assert(interfaceId < mBBInterfaces.size());
        BBInterface& bbInterface = mBBInterfaces[interfaceId];
        bbInterface.pop(data, length);
    }

    BBInterface& getBBInterface(uint32_t interfaceId)
    {
        return mBBInterfaces[interfaceId];
    }

    std::array<BBInterface, 48> mBBInterfaces;
};
