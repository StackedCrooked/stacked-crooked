#include "PhysicalInterface.h"


PhysicalInterface::PhysicalInterface()
{
    mBBInterfaces.resize(64);
}


void PhysicalInterface::pop(const std::vector<RxPacket>& packets)
{
    for (const RxPacket& packet : packets)
    {
        BBInterface& bbInterface = mBBInterfaces[packet.mVlanId];
        bbInterface.pop_later(packet, mBatchedBBInterfaces);
    }

    for (BBInterface* bb_interface : mBatchedBBInterfaces)
    {
        bb_interface->pop_now();
    }

    mBatchedBBInterfaces.clear();
}
