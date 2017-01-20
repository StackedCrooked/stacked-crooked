#include "BBInterface.h"


BBPort& BBInterface::addPort(MACAddress localMAC)
{
    mBBPorts.emplace_back(localMAC);
    return mBBPorts.back();
}


