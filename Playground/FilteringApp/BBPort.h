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

    UDPFlow& getUDPFlow(uint32_t index);
    void addUDPFlow(uint16_t dst_port);

    void pop(RxPacket packet)
    {
        if (is_local_mac(packet))
        {
            mUnicastCounter++;

            for (UDPFlow& flow : mUDPFlows)
            {
                if (flow.match(packet, mLayer3Offset))
                {
                    flow.accept(packet);
                    mUDPAccepted++;
                    return;
                }
            }
        }
        else if (is_broadcast(packet))
        {
            mBroadcastCounter++;
        }
        else if (is_multicast(packet))
        {
            mMulticastCounter++;
        }
        else
        {
            mInvalidDestination++;
            return;
        }

        mStack.add_to_queue(packet);
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
        return Decode<IPv4Header>(ip_data).mProtocolId == ProtocolId::UDP;
    }

    LocalMAC mLocalMAC;
    uint16_t mLayer3Offset = sizeof(EthernetHeader); // default
    uint64_t mUnicastCounter = 0;
    uint64_t mMulticastCounter = 0;
    uint64_t mBroadcastCounter = 0;
    uint64_t mInvalidDestination = 0;
    uint64_t mUDPAccepted = 0;
    uint64_t mTCPAccepted = 0;
    std::vector<UDPFlow> mUDPFlows;
    Stack mStack;
    uint64_t mPadding[2];
};


