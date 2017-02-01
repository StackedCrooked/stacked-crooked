#include "PhysicalInterface.h"


void PhysicalInterface::pop(const std::vector<RxPacket>& packets)
{
    for (const RxPacket& packet : packets)
    {
        BBInterface& bbInterface = mBBInterfaces[packet.mVlanId];
        bbInterface.pop(packet);
    }
}
