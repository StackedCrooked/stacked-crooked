#include "BBServer.h"


void BBServer::run(std::vector<std::vector<uint8_t> >& batch, uint32_t num_packets)
{
    assert(batch.size() == 32);
    for (auto i = 0u; i != num_packets; ++i)
    {
        mPhysicalInterface.pop(batch[i % 32].data(), batch[i % 32].size(), 0);
    }
}
