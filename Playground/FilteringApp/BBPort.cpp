#include "BBPort.h"


BBPort::BBPort(MACAddress local_mac) :
    mLocalMAC(local_mac),
    mLocalIP(1, 1, 1, local_mac[5])
{
    mStack.mPackets.reserve(1024);
}


void BBPort::addUDPFlow(uint16_t dst_port)
{
    mUDPFlows.push_back(UDPFlow(IPv4Address(1, 1, 1, 1), mLocalIP, 1, dst_port));
}






