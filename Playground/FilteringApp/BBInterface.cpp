#include "BBInterface.h"
#include "PhysicalInterface.h"


static_assert(sizeof(BBInterface) == 64, "");


BBPort& BBInterface::addPort(PhysicalInterface& physicalInterface, MACAddress localMAC)
{
    mBBPorts.emplace_back(localMAC);
    return physicalInterface.addPort(*this, mBBPorts.back());
}
