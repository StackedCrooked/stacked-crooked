#pragma once


#include "MaskFilter.h"
#include "RxPacket.h"
#include <cstdint>


struct UDPFlow
{
    UDPFlow(IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
        mFilter(ProtocolId::UDP, src_ip, dst_ip, src_port, dst_port)
    {
    }

    void process(RxPacket packet)
    {
        if (mFilter.match(packet.mData, packet.mSize))
        {
            mPacketsReceived++;
            mBytesReceived += packet.mSize;
        }
    }

    MaskFilter mFilter;
    uint64_t mPacketsReceived = 0;
    uint64_t mBytesReceived = 0;
};
