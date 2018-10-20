#include "BBInterface.h"


BBInterface::BBInterface()
{
    mPacketBatch.reserve(32);
}


BBPort& BBInterface::addPort(MACAddress localMAC)
{
    mBBPorts.push_back(std::make_unique<BBPort>(localMAC));
    return *mBBPorts.back();
}
