#include "BBInterface.h"


BBInterface::BBInterface()
{
    mPackets.reserve(32);
}


BBPort& BBInterface::addPort(MACAddress localMAC)
{
    mBBPorts.emplace_back(localMAC);
    return mBBPorts.back();
}
