#include "BBInterface.h"


static_assert(sizeof(BBInterface) == 64, "");


BBPort& BBInterface::addPort(MACAddress localMAC)
{
    mBBPorts.emplace_back(localMAC);
    return mBBPorts.back();
}


