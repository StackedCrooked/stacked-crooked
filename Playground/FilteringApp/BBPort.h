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

    UDPFlow& getUDPFlow(uint32_t index) { return *mUDPFlows[index]; }

    void addUDPFlow(uint16_t dst_port);

    void pop_one(RxPacket packet);

    bool is_ipv4(RxPacket packet) const
    {
        return Decode<EthernetHeader>(packet.data()).mEtherType == Net16(0x0800);
    }

    void pop_many(RxPacket* packets, uint32_t size)
    {
        for (auto i = 0u; i != size; ++i)
        {
            pop_one(packets[i]);
        }
    }

    void handle_other(const RxPacket& packet)
    {
        mStack.add_to_queue(packet);
    }

    bool is_local_mac(const RxPacket& packet) { return mLocalMAC.equals(packet.data()); }
    bool is_broadcast(const RxPacket& packet) { return 0x0000FFFFFFFFFFFF == (Decode<uint64_t>(packet.data()) & 0x0000FFFFFFFFFFFF); }
    bool is_multicast(const RxPacket& packet) { return packet[0] & 0x01; }

    struct Stats
    {
        uint64_t mUnicastCounter = 0;
        uint64_t mBroadcastCounter = 0;
        uint64_t mMulticastCounter = 0;
        uint64_t mUDPAccepted = 0;
    };

    LocalMAC mLocalMAC;
    IPv4Address mLocalIP;
    uint16_t mVLANId = 0; // zero means on vlan id
    uint16_t mLayer3Offset = sizeof(EthernetHeader); // default
    Stats mStats;
    std::vector<std::unique_ptr<UDPFlow>> mUDPFlows;
    Stack mStack;
};
