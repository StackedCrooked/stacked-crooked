#pragma once


#include "Decode.h"
#include "MACAddress.h"
#include "RxPacket.h"
#include "RxTrigger.h"
#include "Stack.h"
#include "UDPFlow.h"
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
        }
        else if (is_broadcast(packet))
        {
            mBroadcastCounter++;
        }

        if (!mRxTriggers.empty())
        {
            // Check packet against all BPF filters.
            for (RxTrigger& rxTrigger : mRxTriggers)
            {
                rxTrigger.process(packet);
            }
        }

        if (get_protocol(packet) == ProtocolId::UDP) // check once
        {
            // Check packet against all UDP flows.
            // TODO: use a hash table
            for (UDPFlow& flow : mUDPFlows)
            {
                if (flow.match(packet, mLayer3Offset)) // BBPort knows its layer-3 offset
                {
                    flow.accept(packet);
                    mUDPAccepted++;
                    // No further processing needed.
                    return;
                }
            }
        }

        mStack.pop(packet);
    }

    bool is_local_mac(const RxPacket& packet)
    {
        return mLocalMAC.equals(packet.data());
    }

    bool is_broadcast(const RxPacket& packet)
    {
        return 0x0000FFFFFFFFFFFF == (Decode<uint64_t>(packet.data()) & 0x0000FFFFFFFFFFFF);
    }

    ProtocolId get_protocol(RxPacket rxPacket)
    {
        auto l3_data = rxPacket.data() + mLayer3Offset;
        return ProtocolId(l3_data[offsetof(IPv4Header, mProtocolId)]);
    }

    LocalMAC mLocalMAC;
    uint16_t mLayer3Offset = sizeof(EthernetHeader); // default
    uint16_t mInterfaceVlanId = 0; // default
    uint64_t mTotalCounter = 0;
    uint64_t mUnicastCounter = 0;
    uint64_t mBroadcastCounter = 0;
    uint64_t mUDPAccepted = 0;
    std::vector<RxTrigger> mRxTriggers;
    std::vector<UDPFlow> mUDPFlows;
    Stack mStack;
};


// Validate size
// Define one or more UDP flows.
// Use MaskFilter
//
