#pragma once


#include "BBPort.h"
#include <array>
#include <cassert>
#include <vector>


struct BBInterface
{
    BBInterface()
    {
    }

    ~BBInterface() = default;

    BBPort& addPort(MACAddress localMAC)
    {
        mBBPorts.emplace_back(localMAC);
        return mBBPorts.back();
    }

    void pop(const uint8_t* data, uint32_t length, std::vector<BBInterface*>& active_interfaces)
    {
        if (__builtin_expect(mBatchSize == 0, 0))
        {
            active_interfaces.push_back(this);
        }

        mBatch[mBatchSize++] = std::pair<const uint8_t*, uint32_t>(data, length);

        if (__builtin_expect(mBatchSize == mBatch.size(), 0))
        {
            pop_now();
        }
    }

    void pop_now();

    std::vector<BBPort> mBBPorts;
    uint32_t mBatchSize = 0;
    std::array<std::pair<const uint8_t*, uint32_t>, 32> mBatch;
};
