#include "BBServer.h"
#include "RxPacket.h"


BBServer::BBServer()
{
}


void BBServer::run(const std::vector<std::vector<uint8_t>>& batch, uint32_t num_packets)
{
    prepare(batch);

    auto num_repeats = num_packets / batch.size();
    for (auto repeat_index = 0u; repeat_index != num_repeats; ++repeat_index)
    {

        auto& ph = getPhysicalInterface(0);

        for (RxPacket& rxPacket : mRxPackets)
        {
            ph.pop(rxPacket);
        }
    }
}


void BBServer::prepare(const std::vector<std::vector<uint8_t> >& batch)
{
    if (mRxPackets.size() != batch.size())
    {
        mRxPackets.resize(batch.size());
        for (RxPacket& rxPacket : mRxPackets)
        {
            auto i = &rxPacket - mRxPackets.data();
            rxPacket = RxPacket(batch[i].data(), batch[i].size(), batch[i][5] - 1);
        }
    }
}
