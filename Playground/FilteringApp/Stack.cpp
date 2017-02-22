#include "Stack.h"
#include "Logger.h"
#include <cstring>


Stack::Stack() :
    mConsumerThread([this]{ run_consumer(); })
{
}


Stack::~Stack()
{
    stop();
    Log() << this << " Stack::mRxPackets=" << mRxPackets << std::endl;
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

        //Log() << this << " mSharedItems.size=" << mSharedItems.size() << std::endl;

        mSharedItems.clear();

        lock.unlock();

        mRxPackets += mConsumerItems.size();
    }
}
