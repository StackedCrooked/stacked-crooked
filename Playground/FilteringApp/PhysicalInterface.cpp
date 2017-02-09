#include "PhysicalInterface.h"


std::vector<BBInterface*> bb_interfaces;

void PhysicalInterface::pop(const std::vector<RxPacket>& packets)
{
    for (const RxPacket& packet : packets)
    {
        mBBInterfaces[packet.mVlanId].pop_later(packet, bb_interfaces);
    }
	for (BBInterface* bb_interface : bb_interfaces)
	{
		bb_interface->pop_now();
	}
	bb_interfaces.clear();
}
