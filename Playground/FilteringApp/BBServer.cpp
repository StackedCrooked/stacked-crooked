#include "BBServer.h"
#include "RxPacket.h"
#include <iostream>


BBServer::BBServer()
{
}


void BBServer::run(const std::vector<std::vector<uint8_t>>& batch, uint32_t num_packets)
{
    assert(batch.size() == 32);
    std::array<RxPacket, 32> rxPackets;
    for (auto& vec : batch)
    {
        auto i = &vec - batch.data();
        rxPackets[i] = RxPacket(vec.data(), vec.size(), 0);
        rxPackets[i].mBBInterfaceId = int(rxPackets[i].data()[5]) - 1;
    }

    auto num_iterations = num_packets / 32;

    for (auto i = 0u; i != num_iterations; ++i)
    {
#if 1
        getPhysicalInterface(0).pop_many(rxPackets.data(), rxPackets.size());
#else
        for (const RxPacket& rxPacket : rxPackets)
        {
            getPhysicalInterface(0).pop(rxPacket);
        }
#endif
    }
}
