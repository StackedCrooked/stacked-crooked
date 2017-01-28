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

    uint64_t getTotalCount() const
    {
        return mUnicastCounter + mBroadcastCounter + mOtherCounter;
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
        else
        {
            mOtherCounter++;
        }

        if (!mRxTriggers.empty())
        {
            process_rx_triggers(packet);
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

        // If we get here then we need to pass it to the stack.
        mStack.add_to_queue(packet);
    }

    void process_rx_triggers(RxPacket packet)
    {
        // Check packet against all BPF filters.
        for (RxTrigger& rxTrigger : mRxTriggers)
        {
            rxTrigger.process(packet);
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

    bool is_udp(RxPacket rxPacket)
    {
        auto l3_data = rxPacket.data() + mLayer3Offset;
        return ProtocolId(l3_data[offsetof(IPv4Header, mProtocolId)]) == ProtocolId::UDP;
    }

    UDPFlow& getUDPFlow(uint32_t index);

    void addUDPFlow(uint16_t dst_port);

    LocalMAC mLocalMAC;
    uint16_t mLayer3Offset = sizeof(EthernetHeader); // default
    uint16_t mInterfaceVlanId = 0; // default
    uint64_t mUnicastCounter = 0;
    uint64_t mBroadcastCounter = 0;
    uint64_t mOtherCounter = 0;
    uint64_t mUDPAccepted = 0;
    std::vector<RxTrigger> mRxTriggers;
    std::vector<UDPFlow> mUDPFlows;
    Stack mStack;
};


// Validate size
// Define one or more UDP flows.
// Use MaskFilter
//
