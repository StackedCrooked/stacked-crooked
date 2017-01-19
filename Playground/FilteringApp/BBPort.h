#pragma once

#include "Decode.h"
#include "MACAddress.h"


struct BBPort
{
    BBPort(MACAddress local_mac) :
        mLocalMAC(local_mac)
    {
    }

    void pop(const uint8_t* data, uint32_t /*length*/)
    {
        auto dst_mac = Decode<MACAddress>(data);

        if (dst_mac != mLocalMAC && dst_mac.isBroadcast() && dst_mac.isMulticast())
        {
            // Reject packet
            return;
        }

        mBroadcastCounter += dst_mac.isBroadcast();
        mUnicastCounter++;

        // pop to stack

    }

    MACAddress mLocalMAC = MACAddress();
    uint64_t mUnicastCounter = 0;
    uint64_t mBroadcastCounter = 0;
};
