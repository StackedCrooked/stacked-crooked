#pragma once


#include "RxPacket.h"
#include "Logger.h"
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
            if (!mMutex.try_lock())
            {
                mFailedLocks++;

                do {
                    asm volatile ("pause;");
                }
                while (!mMutex.try_lock());
            }

            mLocks++;

            std::lock_guard<std::mutex> lock(mMutex, std::adopt_lock);
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
    __attribute__((aligned(64))) uint64_t mFailedLocks = 0;
    __attribute__((aligned(64))) uint64_t mLocks = 0;

    std::mutex mMutex;
    std::condition_variable mCondition;
    std::thread mConsumerThread;
};
