#pragma once


#include "RxPacket.h"
#include <boost/lockfree/spsc_queue.hpp>
#include <array>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <thread>


#include <iostream>


struct Stack
{
    Stack();

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    ~Stack();

    void stop();

    void pop_later(RxPacket rxPacket)
    {
        mProducerItems.push_back(rxPacket);
    }

    void pop_now()
    {
        if (mProducerItems.empty())
        {
            return;
        }

        {
            std::unique_lock<std::mutex> lock(mMutex);
            std::swap(mProducerItems, mSharedItems);
        }

        mCondition.notify_one();
    }

private:
    void run_consumer();

    __attribute__((aligned(64))) bool mQuit = false;
    __attribute__((aligned(64))) std::vector<RxPacket> mProducerItems;
    __attribute__((aligned(64))) std::vector<RxPacket> mSharedItems;
    __attribute__((aligned(64))) std::vector<RxPacket> mConsumerItems;
    __attribute__((aligned(64))) uint64_t mRxPackets = 0;

    std::mutex mMutex;
    std::condition_variable mCondition;
    std::thread mConsumerThread;
};
