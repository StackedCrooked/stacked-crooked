#include "BBServer.h"
#include "RxPacket.h"
#include <iostream>


BBServer::BBServer()
{
}

std::vector<RxPacket> rxpackets;

void BBServer::run(const std::vector<std::vector<uint8_t>>& batch, uint32_t num_packets)
{
    auto num_repeats = num_packets / batch.size();
    if (!rxpackets.empty())
    {
        rxpackets.resize(batch.size());
        for (RxPacket& rxPacket : rxpackets)
        {
            auto i = &rxPacket - rxpackets.data();
            rxPacket = RxPacket(batch[i].data(), batch[i].size(), batch[i][5] - 1);
        }
    }

    for (RxPacket& rxPacket : rxpackets)
    {
        auto i = &rxPacket - rxpackets.data();
        rxPacket = RxPacket(batch[i].data(), batch[i].size(), batch[i][5] - 1);
    }

    auto& ph = getPhysicalInterface(0);

    for (auto repeat_index = 0u; repeat_index != num_repeats; ++repeat_index)
    {
        for (auto i = 0u; i != batch.size(); i += 2u)
        {
            ph.pop(RxPacket(batch[i + 0].data(), batch[i + 0].size(), batch[i + 0].data()[5] - 1));
            ph.pop(RxPacket(batch[i + 1].data(), batch[i + 1].size(), batch[i + 1].data()[5] - 1));
        }

//        for (const std::vector<uint8_t>& packet_vec : batch)
//        {
//            ph.pop(RxPacket(packet_vec.data(), packet_vec.size(), packet_vec.data()[5] - 1));
//        }
    }
}
