#pragma once


#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>


struct Packet
{
    uint32_t mSize = 0;
};


struct Queue
{
    Queue();

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    ~Queue();

    void stop();

    void push(Packet value)
    {
        auto was_waiting = false;

        {
            if (!mMutex.try_lock())
            {
                mMutex.lock();
                mTxBlocks++;
            }
            mTxLocks++;

            std::lock_guard<std::mutex> lock(mMutex, std::adopt_lock);
            mRxReceived += value.mSize;
            mItems1.push_back(value);
            if (mWaiting)
            {
                was_waiting = true;
                mWaiting = false;
            }
        }

        if (was_waiting)
        {
            mCondition.notify_one();
            mNotifies++;
        }
    }

private:
    void run();

    __attribute__((aligned(64))) bool mQuit = false;
    __attribute__((aligned(64))) bool mWaiting = true;
    __attribute__((aligned(64))) uint32_t mTxBlocks = 0;
    __attribute__((aligned(64))) uint32_t mTxLocks = 0;
    __attribute__((aligned(64))) uint32_t mRxBlocks = 0;
    __attribute__((aligned(64))) uint32_t mRxLocks = 0;
    __attribute__((aligned(64))) uint32_t mNotifies = 0;
    __attribute__((aligned(64))) uint32_t mSwaps = 0;
    __attribute__((aligned(64))) uint32_t mRxReceived = 0;
    __attribute__((aligned(64))) uint32_t mRxProcessed = 0;
    __attribute__((aligned(64))) std::vector<Packet> mItems1;
    __attribute__((aligned(64))) std::vector<Packet> mItems2;
    __attribute__((aligned(64))) std::mutex mMutex;
    __attribute__((aligned(64))) std::condition_variable mCondition;
    std::thread mThread;
};
