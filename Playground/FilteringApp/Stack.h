#pragma once


#include "RxPacket.h"
#include <boost/lockfree/spsc_queue.hpp>
#include <array>
#include <vector>
#include <iostream>
#include <thread>


struct Stack
{
    Stack();

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    ~Stack();

    void stop();

    void pop_later(RxPacket rxPacket)
    {
        mBatch.push_back(rxPacket);
    }

    void pop_now()
    {
        if (!mBatch.empty())
        {
            mPackets.push(mBatch.data(), mBatch.size());
            std::cout << mBatch.size() << std::endl;
            mBatch.clear();
        }
    }

private:
    void run_consumer();

    __attribute__((aligned(64)))
    boost::lockfree::spsc_queue<RxPacket, boost::lockfree::capacity<256>> mPackets;

    __attribute__((aligned(64)))
    std::vector<RxPacket> mBatch;

    uint64_t mRxPackets = 0;
    std::thread mConsumerThread;
};
