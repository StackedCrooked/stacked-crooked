#include "BBPort.h"


BBPort::BBPort(MACAddress local_mac) :
    mLocalMAC(local_mac),
    mLocalIP(1, 1, 1, local_mac[5])
{
    mStack.mPackets.reserve(1024);
}


void BBPort::addUDPFlow(uint16_t dst_port)
{
    mUDPFlows.push_back(std::make_unique<UDPFlow>(IPv4Address(1, 1, 1, 1), mLocalIP, 1, dst_port));
}


void BBPort::pop(RxPacket packet)
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

    if (is_ipv4(packet))
    {
        for (auto& flow : mUDPFlows)
        {
            if (flow->match(packet, mLayer3Offset)) // BBPort knows its layer-3 offset
            {
                flow->accept(packet);
                mStats.mUDPAccepted++;
                return;
            }
        }

        // If we didn't match any UDP flows then the IP may wrong. So we still need to check it.
        auto dst_ip = Decode<IPv4Header>(packet.data() + mLayer3Offset).mDestinationIP;

        if (dst_ip != mLocalIP && !dst_ip.isBroadcast() && !dst_ip.isMulticast())
        {
            // Invalid destination IP.
            return;
        }
    }

    // Send it to the stack.
    handle_other(packet);
}
