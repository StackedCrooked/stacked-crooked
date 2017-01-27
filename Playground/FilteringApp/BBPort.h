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
        if (mLocalMAC.equals(packet.data()))
        {
            mUnicastCounter++;
        }
        else if (is_broadcast(packet))
        {
            mBroadcastCounter++;
        }

        process(packet);
    }

    bool is_local_mac(const RxPacket& packet)
    {
        return mLocalMAC.equals(packet.data());
    }

    bool is_broadcast(const RxPacket& packet)
    {
        return 0x0000FFFFFFFFFFFF == (Decode<uint64_t>(packet.data()) & 0x0000FFFFFFFFFFFF);
    }

    static ProtocolId get_protocol(RxPacket rxPacket)
    {
        return ProtocolId(rxPacket[offsetof(IPv4Header, mProtocolId)]);
    }

    void process(RxPacket packet)
    {
        if (!mRxTriggers.empty())
        {
            // All packets must be checked against all triggers.
            pop_rx_triggers(packet);
        }

        if (get_protocol(packet) == ProtocolId::UDP)
        {
            for (UDPFlow& flow : mUDPFlows)
            {
                if (flow.match(packet, mLayer3Offset))
                {
                    flow.accept(packet);
                    // No further processing needed.
                    return;
                }
            }
        }

        mStack.pop(packet);
    }

    void pop_rx_triggers(RxPacket packet);

    LocalMAC mLocalMAC;
    uint16_t mLayer3Offset = sizeof(EthernetHeader); // default
    uint16_t mInterfaceVlanId = 0; // default
    uint64_t mUnicastCounter = 0;
    uint64_t mBroadcastCounter = 0;
    uint64_t mTotalCounter = 0;
    std::vector<RxTrigger> mRxTriggers;
    std::vector<UDPFlow> mUDPFlows;
    Stack mStack;
};


// Validate size
// Define one or more UDP flows.
// Use MaskFilter
//
