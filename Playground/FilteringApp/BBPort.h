#pragma once


#include "Counter.h"
#include "Decode.h"
#include "Features.h"
#include "Likely.h"
#include "MACAddress.h"
#include "RxPacket.h"
#include "RxTrigger.h"
#include "Stack.h"
#include "UDPFlow.h"
#include <vector>


namespace Features {

}


struct BBPort
{
    BBPort(MACAddress local_mac);

    UDPFlow& getUDPFlow(uint32_t index) { return mUDPFlows[index]; }

    void addUDPFlow(uint16_t dst_port);

    void pop(RxPacket packet)
    {
        if (is_local_mac(packet))
        {
            mStats.mUnicastCounter++;
        }
        else if (is_broadcast(packet))
        {
            mStats.mBroadcastCounter++;
        }
        else if (is_multicast(packet))
        {
            mStats.mMulticastCounter++;
        }
        else
        {
            return;
        }

        if (has_ethertype_ipv4(packet))
        {
            // If we didn't match any UDP flows then the IP may wrong. So we still need to check it.
            if (Features::enable_ip_check)
            {
                auto dst_ip = Decode<IPv4Header>(packet.data() + mLayer3Offset).mDestinationIP;
                if (dst_ip != mLocalIP && !dst_ip.isBroadcast() && !dst_ip.isMulticast())
                {
                    // Invalid destination IP.
                    return;
                }
            }

            if (Features::enable_udp)
            {
                for (UDPFlow& flow : mUDPFlows)
                {
                    if (flow.match(packet, mLayer3Offset)) // BBPort knows its layer-3 offset
                    {
                        flow.accept(packet);
                        mStats.mUDPAccepted++;
                        return;
                    }
                }
            }
        }

        // Send it to the stack.
        if (Features::enable_tcp)
        {
            mStack.add_to_queue(packet);
        }
    }

    bool is_local_mac(const RxPacket& packet)
    {
        if (!Features::enable_mac_check) return true;
        return mLocalMAC.equals(packet.data());
    }

    bool is_broadcast(const RxPacket& packet)
    {
        if (!Features::enable_mac_check) return true;
        return 0x0000FFFFFFFFFFFF == (Decode<uint64_t>(packet.data()) & 0x0000FFFFFFFFFFFF);
    }

    bool is_multicast(const RxPacket& packet)
    {
        if (!Features::enable_mac_check) return true;
        return packet[0] & 0x01;
    }

    bool has_ethertype_ipv4(RxPacket packet) const
    {
        if (!Features::enable_ip_check) return true;
        return Decode<EthernetHeader>(packet.data()).mEtherType == Net16(0x0800);
    }

    void pop_many(RxPacket* packets, uint32_t size)
    {
        for (auto i = 0u; i != size; ++i)
        {
            pop(packets[i]);
        }
    }

    struct Stats
    {
        Counter mUnicastCounter = 0;
        Counter mBroadcastCounter = 0;
        Counter mMulticastCounter = 0;
        uint64_t mUDPAccepted = 0;
    };

    LocalMAC mLocalMAC;
    IPv4Address mLocalIP;
    uint16_t mVLANId = 0; // zero means on vlan id
    uint16_t mLayer3Offset = sizeof(EthernetHeader); // default
    Stats mStats;
    std::vector<UDPFlow> mUDPFlows;
    Stack mStack;
};
