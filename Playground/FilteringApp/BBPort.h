#pragma once


#include "Decode.h"
#include "MACAddress.h"


struct BBPort
{
    BBPort(MACAddress local_mac);

    void pop(const uint8_t* data, uint32_t /*length*/)
    {
        auto dst_mac = Decode<MACAddress>(data);

        if (__builtin_expect(mLocalMAC == dst_mac, 1))
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
