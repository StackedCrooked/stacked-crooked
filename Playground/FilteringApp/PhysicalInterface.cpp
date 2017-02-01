#include "PhysicalInterface.h"


void PhysicalInterface::pop(const std::vector<RxPacket>& packet_vec)
{
    const RxPacket* packets = packet_vec.data();
    uint32_t length = packet_vec.size();


    while (length >= 4)
    {
        std::size_t hashes[4] = { 0ull, 0ull, 0ull, 0ull };

        boost::hash_combine(hashes[0], Decode<uint32_t>(packets[0].mData + 0));
        boost::hash_combine(hashes[1], Decode<uint32_t>(packets[1].mData + 0));
        boost::hash_combine(hashes[2], Decode<uint32_t>(packets[2].mData + 0));
        boost::hash_combine(hashes[3], Decode<uint32_t>(packets[3].mData + 0));

        boost::hash_combine(hashes[0], Decode<uint16_t>(packets[0].mData + 4));
        boost::hash_combine(hashes[1], Decode<uint16_t>(packets[1].mData + 4));
        boost::hash_combine(hashes[2], Decode<uint16_t>(packets[2].mData + 4));
        boost::hash_combine(hashes[3], Decode<uint16_t>(packets[3].mData + 4));

        boost::hash_combine(hashes[0], packets[0].mVlanId);
        boost::hash_combine(hashes[1], packets[1].mVlanId);
        boost::hash_combine(hashes[2], packets[2].mVlanId);
        boost::hash_combine(hashes[3], packets[3].mVlanId);

        std::array<BBPort*, 4> ports;

        ports[0] = mBBPortIndices[hashes[0] % 128];
        ports[1] = mBBPortIndices[hashes[1] % 128];
        ports[2] = mBBPortIndices[hashes[2] % 128];
        ports[3] = mBBPortIndices[hashes[3] % 128];

        for (auto i = 0ul; i != 4ul; ++i)
        {
            ports[i]->pop(packets[i]);
        }

        packets += 4;
        length -= 4;
    }

    assert(length == 0);
}


BBPort&PhysicalInterface::addPort(BBInterface& bbInterface, BBPort& bbPort)
{
    auto interfaceId = &bbInterface - mBBInterfaces.data();

    std::size_t hash = 0;
    boost::hash_combine(hash, Decode<uint32_t>(bbPort.mLocalMAC.data() + 0));
    boost::hash_combine(hash, Decode<uint16_t>(bbPort.mLocalMAC.data() + 4));
    boost::hash_combine(hash, interfaceId);
    mBBPortIndices[hash % mBBPortIndices.size()] = &bbPort;

    return bbPort;
}
