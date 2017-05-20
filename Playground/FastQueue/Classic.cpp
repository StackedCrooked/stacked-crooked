#include "Classic.h"


Queue::Queue() :
    mThread(&Queue::run, this)
{
    mItems1.reserve(1024);
    mItems2.reserve(1024);
}


Queue::~Queue()
{
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mQuit = true;
        mCondition.notify_all();
    }

    mThread.join();

    std::cout
        << "mRxReceived=" << mRxReceived
        << " mRxProcessed=" << mRxProcessed
        << " mItems1.size=" << mItems1.size()
        << " mItems2.size=" << mItems2.size()
        << " mNotifies=" << mNotifies
        << " mSwaps=" << mSwaps
        << std::endl;
}


void Queue::run()
{
    for (;;)
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);

            if (mQuit)
            {
                return;
            }

            if (mItems1.empty())
            {
                mWaiting = true;
                mCondition.wait(lock, [&] {
                    return mQuit || !mItems1.empty();
                });
            }

            std::swap(mItems1, mItems2);
            mSwaps++;
        }

        for (Packet& p : mItems2)
        {
            mRxProcessed += p.mSize;
        }
        mItems2.clear();
    }
}
