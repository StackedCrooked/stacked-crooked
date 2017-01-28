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
    //BBPort* mNextPort = nullptr;

    BBPort(MACAddress local_mac);

    uint64_t getTotalCount() const
    {
        return mUnicastCounter + mBroadcastCounter + mInvalidDestination;
    }

    void pop(RxPacket packet)
    {
        if (is_local_mac(packet))
        {
            mUnicastCounter++;
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

        if (is_udp(packet))
        {
            // Check packet against all UDP flows.
            // TODO: use a hash table
            for (UDPFlow& flow : mUDPFlows)
            {
                if (flow.match(packet, mLayer3Offset)) // BBPort knows its layer-3 offset
                {
                    flow.accept(packet);
                    mUDPAccepted++;
                    return;
                }
            }
        }

        // Pass the packet to the stack
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

    bool is_udp(RxPacket rxPacket)
    {
        auto ip_data = rxPacket.data() + mLayer3Offset;
        return Decode<IPv4Header>(ip_data).mProtocolId == ProtocolId::UDP;
    }

    UDPFlow& getUDPFlow(uint32_t index);

    void addUDPFlow(uint16_t dst_port);

    LocalMAC mLocalMAC;
    uint16_t mLayer3Offset = sizeof(EthernetHeader); // default
    uint16_t mInterfaceVlanId = 0; // default
    uint64_t mUnicastCounter = 0;
    uint64_t mMulticastCounter = 0;
    uint64_t mBroadcastCounter = 0;
    uint64_t mInvalidDestination = 0;
    uint64_t mUDPAccepted = 0;
    std::vector<UDPFlow> mUDPFlows;
    Stack mStack;
    uint64_t mPadding[3];
};


// Validate size
// Define one or more UDP flows.
// Use MaskFilter
//
