#pragma once


#include "UDPFlow.h"
#include "RxPacket.h"
#include "MACAddress.h"
#include "Decode.h"
#include "Likely.h"
#include <vector>



struct BBPort
{
    BBPort(MACAddress local_mac);

    void addUDPFlow(uint16_t dst_port);
    UDPFlow& getUDPFlow(uint32_t i)
    {
        return mUDPFlows[i];
    }

    void pop(RxPacket packet)
    {
        if (check_mac(packet))
        {
            mUnicastCounter++;
        }
        else if (is_broadcast(packet))
        {
            mBroadcastCounter++;
        }

        for (UDPFlow& flow : mUDPFlows)
        {
            flow.process(packet);
        }
    }

    bool check_mac(const RxPacket& packet)
    {
        return (*reinterpret_cast<const uint32_t*>(mLocalMAC.data() + 2) == *reinterpret_cast<const uint32_t*>(packet.data() + 2))
             & (*reinterpret_cast<const uint16_t*>(mLocalMAC.data() + 0) == *reinterpret_cast<const uint16_t*>(packet.data() + 0))
        ;
    }

    bool is_broadcast(const RxPacket& packet)
    {
        return (0xFFFFFFFF == *reinterpret_cast<const uint32_t*>(packet.data() + 2))
             & (0x0000FFFF == *reinterpret_cast<const uint16_t*>(packet.data() + 0))
        ;
    }

    void pop_many(const RxPacket* packet, uint32_t length)
    {
        while (length >= 4)
        {
            pop(packet[0]);
            pop(packet[1]);
            pop(packet[2]);
            pop(packet[3]);
            length -= 4;
            packet += 4;
        }

        for (auto i = 0u; i != length; ++i)
        {
            pop(packet[i]);
        }
    }

    MACAddress mLocalMAC = MACAddress();
    uint64_t mUnicastCounter = 0;
    uint64_t mBroadcastCounter = 0;
    std::vector<UDPFlow> mUDPFlows;

};


// Validate size
// Define one or more UDP flows.
// Use MaskFilter
