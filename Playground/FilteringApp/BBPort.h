#pragma once


#include "RxPacket.h"
#include "Likely.h"
#include "Decode.h"
#include "MACAddress.h"


struct BBPort
{
    BBPort(MACAddress local_mac);

    void pop(RxPacket packet)
    {
        auto dst_mac = Decode<MACAddress>(packet.mData);

        if (mLocalMAC == dst_mac)
        {
            mUnicastCounter++;
        }
        else if (dst_mac.isBroadcast())
        {
            mBroadcastCounter++;
        }
    }

    MACAddress mLocalMAC = MACAddress();
    uint64_t mUnicastCounter = 0;
    uint64_t mBroadcastCounter = 0;
};


// Validate size
// Define one or more UDP flows.
// Use MaskFilter
