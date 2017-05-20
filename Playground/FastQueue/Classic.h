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

    void push(Packet value)
    {
        auto was_waiting = false;

        {
            std::lock_guard<std::mutex> lock(mMutex);
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

    bool mQuit = false;
    bool mWaiting = true;
    uint32_t mNotifies = 0;
    uint32_t mSwaps = 0;
    uint32_t mRxReceived = 0;
    uint32_t mRxProcessed = 0;
    std::vector<Packet> mItems1;
    std::vector<Packet> mItems2;
    std::mutex mMutex;
    std::condition_variable mCondition;
    std::thread mThread;
};
