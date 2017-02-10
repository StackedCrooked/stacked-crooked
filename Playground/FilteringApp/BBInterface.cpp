#include "BBInterface.h"


static_assert(sizeof(BBInterface) == 128, "");


BBInterface::BBInterface()
{
    mPackets.reserve(32);
}


BBPort& BBInterface::addPort(MACAddress localMAC)
{
    mBBPorts.emplace_back(localMAC);
    return mBBPorts.back();
}


void BBInterface::pop_now()
{
    for (BBPort& port : mBBPorts)
    {
        port.pop_many(mPackets.data(), mPackets.size());
    }
    mPackets.clear();
}
