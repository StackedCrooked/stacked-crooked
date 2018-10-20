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

    void pop(const RxPacket* packets, uint32_t size)
    {
        for (auto i = 0u; i != size; ++i)
        {
            const RxPacket& packet = packets[i];

            if (packet.mVlanId < mBBInterfaces.size())
            {
                BBInterface& bbInterface = mBBInterfaces[packet.mVlanId];
                bbInterface.pop(packet);
            }
        }
    }

    BBInterface& getBBInterface(uint32_t i)
    {
        return mBBInterfaces[i];
    }

    std::vector<BBInterface> mBBInterfaces;
};
