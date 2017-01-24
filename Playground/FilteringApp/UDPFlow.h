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
        if (match(packet))
        {
            accept(packet);
        }
    }

    bool match(RxPacket packet)
    {
        return mFilter.match(packet.data(), packet.size());
    }

    void accept(RxPacket packet)
    {
        mPacketsReceived++;
        mBytesReceived += packet.mSize;
    }

    void accept_4(const RxPacket* packet)
    {
        mPacketsReceived += 4;
        mBytesReceived += packet[0].size();
        mBytesReceived += packet[1].size();
        mBytesReceived += packet[2].size();
        mBytesReceived += packet[3].size();
    }

    MaskFilter mFilter;
    uint64_t mPacketsReceived = 0;
    uint64_t mBytesReceived = 0;
};
