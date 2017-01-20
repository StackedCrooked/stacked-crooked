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

    void pop(RxPacket packet)
    {
        auto dst_mac = Decode<MACAddress>(packet.mData);

        if (LIKELY(mLocalMAC == dst_mac))
        {
            mUnicastCounter++;
        }
        else if (dst_mac.isBroadcast())
        {
            mBroadcastCounter++;
        }

        for (UDPFlow& flow : mUDPFlows)
        {
            flow.process(packet);
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
