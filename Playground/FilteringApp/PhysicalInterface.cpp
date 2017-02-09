#include "PhysicalInterface.h"


void PhysicalInterface::pop(const std::vector<RxPacket>& packets)
{
    for (const RxPacket& packet : packets)
    {
        mBBInterfaces[packet.mVlanId].pop_later(packet, mActiveInterfaces);
    }

    for (BBInterface* bb_interface : mActiveInterfaces)
    {
        bb_interface->pop_now();
    }

    mActiveInterfaces.clear();
}
