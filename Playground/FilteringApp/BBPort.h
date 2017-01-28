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

    void pop(RxPacket packet);

    void process_rx_triggers(RxPacket packet);

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
    uint32_t mLayer3Offset = sizeof(EthernetHeader); // default
    uint32_t mInterfaceVlanId = 0; // default
    uint64_t mUnicastCounter = 0;
    uint64_t mBroadcastCounter = 0;
    uint64_t mOtherCounter = 0;
    uint64_t mUDPAccepted = 0;
    std::vector<RxTrigger> mRxTriggers;
    std::vector<UDPFlow> mUDPFlows;
    Stack mStack;
}

__attribute__((aligned(32)));


// Validate size
// Define one or more UDP flows.
// Use MaskFilter
//
