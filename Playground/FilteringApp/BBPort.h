#pragma once


#include "UDPFlow.h"
#include "RxPacket.h"
#include "MACAddress.h"
#include "Decode.h"
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
        mTotalCounter++;

        if (mLocalMAC.equals(packet.data()))
        {
            mUnicastCounter++;

            for (UDPFlow& flow : mUDPFlows)
            {
                flow.process(packet);
            }
        }
        else if (is_broadcast(packet))
        {
            mBroadcastCounter++;
        }
    }


    bool is_local_mac(const RxPacket& packet)
    {
        return mLocalMAC.equals(packet.data());
    }

    bool is_broadcast(const RxPacket& packet)
    {
        return 0x0000FFFFFFFFFFFF == (Decode<uint64_t>(packet.data()) & 0x0000FFFFFFFFFFFF);
    }

    void pop_many(const RxPacket* packets, uint32_t length);
    void udp_pop_4(const RxPacket* packet);

    LocalMAC mLocalMAC;
    uint64_t mUnicastCounter = 0;
    uint64_t mTotalCounter = 0;
    uint64_t mBroadcastCounter = 0;
    std::vector<UDPFlow> mUDPFlows;

};


inline void BBPort::pop_many(const RxPacket* packets, uint32_t length)
{
    while (length >= 4)
    {
        if (is_local_mac(packets[0]) && is_local_mac(packets[1]) && is_local_mac(packets[2]) && is_local_mac(packets[3]))
        {
            mUnicastCounter += 4;
            udp_pop_4(packets);
        }
        else
        {
            pop(packets[0]);
            pop(packets[1]);
            pop(packets[2]);
            pop(packets[3]);
        }

        length -= 4;
        packets += 4;
    }

    for (auto i = 0u; i != length; ++i)
    {
        pop(packets[i]);
    }
}


inline void BBPort::udp_pop_4(const RxPacket* packet)
{
    for (UDPFlow& flow : mUDPFlows)
    {
        if (flow.match(packet[0]) && flow.match(packet[1]) && flow.match(packet[2]) && flow.match(packet[3]))
        {
            flow.accept_4(packet);
        }
        else
        {
            flow.process(packet[0]);
            flow.process(packet[1]);
            flow.process(packet[2]);
            flow.process(packet[3]);
        }
    }
}


// Validate size
// Define one or more UDP flows.
// Use MaskFilter
//
