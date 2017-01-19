#pragma once


#include "Decode.h"
#include "MACAddress.h"


struct BBPort
{
    BBPort(MACAddress local_mac);

    void pop(const uint8_t* data, uint32_t /*length*/)
    {
        auto dst_mac = Decode<MACAddress>(data);

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
