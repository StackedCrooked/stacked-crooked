#include "BBServer.h"
#include "RxPacket.h"


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
    }

    auto num_iterations = num_packets / 32;

    for (auto i = 0u; i != num_iterations; ++i)
    {
        for (auto& rxPacket : rxPackets)
        {
            mPhysicalInterface.pop(rxPacket);
        }
    }
}
