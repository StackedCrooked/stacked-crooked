#include "BBPort.h"


BBPort::BBPort(MACAddress local_mac) :
    mLocalMAC(local_mac)
{
}


void BBPort::handle_udp(const RxPacket& packet)
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


void BBPort::handle_tcp(const RxPacket& /*packet*/)
{
    mTCPAccepted++;

}


void BBPort::addUDPFlow(uint16_t dst_port)
{
    mUDPFlows.push_back(UDPFlow(IPv4Address(1, 1, 1, 1), IPv4Address(1, 1, 1, 1), 1, dst_port));
}


UDPFlow& BBPort::getUDPFlow(uint32_t index)
{
    return mUDPFlows[index];
}
