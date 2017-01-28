#include "BBPort.h"


BBPort::BBPort(MACAddress local_mac) :
    mLocalMAC(local_mac)
{
}


void BBPort::pop(RxPacket packet)
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


void BBPort::process_rx_triggers(RxPacket packet)
{
    // Check packet against all BPF filters.
    for (RxTrigger& rxTrigger : mRxTriggers)
    {
        rxTrigger.process(packet);
    }
}


void BBPort::addUDPFlow(uint16_t dst_port)
{
    mUDPFlows.push_back(UDPFlow(IPv4Address(1, 1, 1, 1), IPv4Address(1, 1, 1, 1), 1, dst_port));
}


UDPFlow& BBPort::getUDPFlow(uint32_t index)
{
    return mUDPFlows[index];
}
