#pragma once


#include "MaskFilter.h"
#include <cstdint>


struct UDPFlow
{
    UDPFlow(IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
        mFilter(ProtocolId::UDP, src_ip, dst_ip, src_port, dst_port)
    {
    }

    MaskFilter mFilter;
    uint64_t mPacketsReceived;
    uint64_t mBytesReceived;
};
