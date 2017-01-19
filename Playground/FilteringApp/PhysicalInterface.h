#pragma once


#include "BBInterface.h"
#include <array>
#include <cassert>
#include <vector>



struct PhysicalInterface
{
    PhysicalInterface()
    {
        mActiveBBInterfaces.reserve(48);
    }

    PhysicalInterface(const PhysicalInterface&) = delete;
    PhysicalInterface& operator=(const PhysicalInterface&) = delete;

    ~PhysicalInterface()
    {
    }

    void pop(const uint8_t* data, uint32_t length, uint32_t interfaceId)
    {
        //assert(interfaceId < mBBInterfaces.size());
        BBInterface& bbInterface = mBBInterfaces[interfaceId];
        bbInterface.pop(data, length, mActiveBBInterfaces);
    }

    void pop_now();

    BBInterface& getBBInterface(uint32_t interfaceId)
    {
        return mBBInterfaces[interfaceId];
    }

    std::array<BBInterface, 48> mBBInterfaces;
    std::vector<BBInterface*> mActiveBBInterfaces;
};
