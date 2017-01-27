#include "BBServer.h"
#include "RxPacket.h"
#include <iostream>


BBServer::BBServer()
{
}

std::vector<RxPacket> rxpackets;


void BBServer::run(const std::vector<std::vector<uint8_t>>& batch, uint32_t num_packets)
{
    if (rxpackets.size() != batch.size())
    {
        rxpackets.resize(batch.size());
        for (RxPacket& rxPacket : rxpackets)
        {
            auto i = &rxPacket - rxpackets.data();
            rxPacket = RxPacket(batch[i].data(), batch[i].size(), batch[i][5] - 1);
        }
    }

    auto num_repeats = num_packets / batch.size();
    for (auto repeat_index = 0u; repeat_index != num_repeats; ++repeat_index)
    {

        auto& ph = getPhysicalInterface(0);

        for (RxPacket& rxPacket : rxpackets)
        {
            ph.pop(rxPacket);
        }
    }
}
