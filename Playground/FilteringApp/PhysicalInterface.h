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
        mBBInterfaces[packet.mBBInterfaceId].pop(packet);
    }

    void pop_many(const RxPacket* packet_ptr, uint32_t length)
    {
        auto b = packet_ptr;
        const auto e = packet_ptr + length;

        for (;;)
        {
            auto interfaceId = b->mBBInterfaceId;
            auto m = std::find_if(b + 1, e, [&](const RxPacket& packet) {
                return packet.mBBInterfaceId != interfaceId;
            });

            getBBInterface(interfaceId).pop_many(b, m - b);

            if (m == e)
            {
                return;
            }

            b = m;
        }
    }

    BBInterface& getBBInterface(uint32_t i)
    {
        return mBBInterfaces[i];
    }

    std::array<BBInterface, 48> mBBInterfaces;
};
