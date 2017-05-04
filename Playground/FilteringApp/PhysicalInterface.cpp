#include "PhysicalInterface.h"


PhysicalInterface::PhysicalInterface()
{
}


void PhysicalInterface::pop(const std::vector<RxPacket>& packets)
{
    //std::cout << "packets.size=" << packets.size() << std::endl;

    auto packets_ptr = packets.data();
    auto packets_len = packets.size();

    while (packets_len >= 32)
    {
        for (auto b = packets_ptr, e = packets_ptr + 32; b != e; ++b)
        {
            const RxPacket& packet = *b;
            mBBInterfaces[packet.mVlanId].pop_later(packet, mActiveInterfaces);
        }

        for (BBInterface* bb_interface : mActiveInterfaces)
        {
            bb_interface->pop_now();
        }

        packets_ptr += 32;
        packets_len -= 32;
        mActiveInterfaces.clear();
    }
    //std::cout << "remaining packets_len=" << packets_len << std::endl;

    for (auto b = packets_ptr, e = packets_ptr + packets_len; b != e; ++b)
    {
        const RxPacket& packet = *b;
        mBBInterfaces[packet.mVlanId].pop_later(packet, mActiveInterfaces);
    }

    for (BBInterface* bb_interface : mActiveInterfaces)
    {
        bb_interface->pop_now();
    }

    mActiveInterfaces.clear();
}
