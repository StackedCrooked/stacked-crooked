#include "BBPort.h"


BBPort::BBPort(MACAddress local_mac) :
    mLocalMAC(local_mac)
{
    mStack.mPackets.reserve(1024);
    (void)mPadding;
}





void BBPort::addUDPFlow(uint16_t dst_port)
{
    mUDPFlows.push_back(UDPFlow(IPv4Address(1, 1, 1, 1), IPv4Address(1, 1, 1, 1), 1, dst_port));
}


UDPFlow& BBPort::getUDPFlow(uint32_t index)
{
    return mUDPFlows[index];
}
