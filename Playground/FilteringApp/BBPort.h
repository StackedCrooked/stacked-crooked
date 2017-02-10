#pragma once


#include "Decode.h"
#include "Likely.h"
#include "MACAddress.h"
#include "RxPacket.h"
#include "RxTrigger.h"
#include "Stack.h"
#include "UDPFlow.h"
#include <vector>


struct BBPort
{
    BBPort(MACAddress local_mac);

    UDPFlow& getUDPFlow(uint32_t index) { return mUDPFlows[index]; }

    void addUDPFlow(uint16_t dst_port);

    bool pop(RxPacket packet)
    {
        if (is_local_mac(packet))
        {
            mUnicastCounter++;
        }
        else if (is_broadcast(packet))
        {
            mBroadcastCounter++;
        }
        else if (!is_multicast(packet))
        {
            mInvalidDestination++;
            return false;
        }

        for (UDPFlow& flow : mUDPFlows)
        {
            if (flow.match(packet, mLayer3Offset)) // BBPort knows its layer-3 offset
            {
                flow.accept(packet);
                mUDPAccepted++;
                return true;
            }
        }

        // If we didn't match any UDP flows then the IP may wrong. So we still need to check it.
        auto dst_ip = Decode<IPv4Header>(packet.data() + mLayer3Offset).mDestinationIP;

        if (dst_ip != mLocalIP && !dst_ip.isBroadcast() && !dst_ip.isMulticast())
        {
            return false;
        }

        if (is_tcp(packet))
        {
            mTCPAccepted++;
        }

        // handled by protocol stack
        handle_other(packet);

        // We should also try popping the packet to the other
        // bbports on the same interface.
        return false;
    }

    void pop_many(RxPacket* packets, uint32_t size)
    {
        for (auto i = 0u; i != size; ++i)
        {
            pop(packets[i]);
        }
    }

    void handle_other(const RxPacket& packet);

    bool is_local_ip(const RxPacket& packet)
    {
        return Decode<IPv4Header>(packet.data() + mLayer3Offset).mDestinationIP == mLocalIP;
    }

    bool is_local_mac(const RxPacket& packet)
    {
        return mLocalMAC.equals(packet.data());
    }

    bool is_broadcast(const RxPacket& packet)
    {
        return 0x0000FFFFFFFFFFFF == (Decode<uint64_t>(packet.data()) & 0x0000FFFFFFFFFFFF);
    }

    bool is_multicast(const RxPacket& packet)
    {
        return packet[0] & 0x01;
    }

    ProtocolId get_protocol(RxPacket rxPacket)
    {
        auto ip_data = rxPacket.data() + mLayer3Offset;
        return Decode<IPv4Header>(ip_data).mProtocolId;
    }

    bool is_udp(RxPacket rxPacket)
    {
        auto ip_data = rxPacket.data() + mLayer3Offset;
        return Decode<IPv4Header>(ip_data).mProtocolId == ProtocolId::UDP;
    }

    bool is_tcp(RxPacket rxPacket)
    {
        auto ip_data = rxPacket.data() + mLayer3Offset;
        return Decode<IPv4Header>(ip_data).mProtocolId == ProtocolId::TCP;
    }

    LocalMAC mLocalMAC;
    IPv4Address mLocalIP;
    uint16_t mLayer3Offset = sizeof(EthernetHeader); // default
    uint64_t mUnicastCounter = 0;
    uint64_t mBroadcastCounter = 0;
    uint64_t mInvalidDestination = 0;
    uint64_t mUDPAccepted = 0;
    uint64_t mTCPAccepted = 0;
    std::vector<UDPFlow> mUDPFlows;
    Stack mStack;
};
