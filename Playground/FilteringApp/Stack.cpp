#include "Stack.h"
#include <cstring>


Stack::Stack() :
    mConsumerThread([this]{ run_consumer(); })
{
}


Stack::~Stack()
{
    stop();
    std::cout << this << " Stack::mRxPackets=" << mRxPackets << std::endl;
}


void Stack::stop()
{
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mQuit = true;
        mCondition.notify_all();
    }
    mConsumerThread.join();
}


void Stack::run_consumer()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mCondition.wait(lock);

        if (mQuit)
        {
            return;
        }

        if (mSharedItems.empty())
        {
            continue;
        }

        std::swap(mSharedItems, mConsumerItems);

        mSharedItems.clear();

        lock.unlock();

        mRxPackets += mSharedItems.size();
    }
}
