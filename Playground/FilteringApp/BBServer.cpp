#include "BBServer.h"


BBServer::BBServer()
{
}

void BBServer::run(std::vector<std::vector<uint8_t>>& batch, uint32_t num_packets)
{
    assert(batch.size() == 32);

    //auto batch_len = batch.size();


    for (auto i = 0u; i != num_packets / 32; ++i)
    {
        auto batch_ptr = batch.data();
        for (auto i = 0; i != 4; ++i)
        {
            mPhysicalInterface.pop(batch_ptr[0].data(), batch_ptr[0].size(), 0);
            mPhysicalInterface.pop(batch_ptr[1].data(), batch_ptr[1].size(), 0);
            mPhysicalInterface.pop(batch_ptr[2].data(), batch_ptr[2].size(), 0);
            mPhysicalInterface.pop(batch_ptr[3].data(), batch_ptr[3].size(), 0);
            mPhysicalInterface.pop(batch_ptr[4].data(), batch_ptr[4].size(), 0);
            mPhysicalInterface.pop(batch_ptr[5].data(), batch_ptr[5].size(), 0);
            mPhysicalInterface.pop(batch_ptr[6].data(), batch_ptr[6].size(), 0);
            mPhysicalInterface.pop(batch_ptr[7].data(), batch_ptr[7].size(), 0);
            batch_ptr += 8;
        }
    }
    mPhysicalInterface.pop_now();
}
