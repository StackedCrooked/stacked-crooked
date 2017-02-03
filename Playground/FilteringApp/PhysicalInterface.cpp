#include "PhysicalInterface.h"


void PhysicalInterface::pop(const std::vector<RxPacket>& packets)
{
    for (const RxPacket& packet : packets)
    {
        mBBInterfaces[packet.mVlanId].pop(packet);
    }
}
