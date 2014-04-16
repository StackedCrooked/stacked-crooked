#include <atomic>
#include <chrono>
#include <list>
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>


typedef std::chrono::high_resolution_clock Clock;

std::mutex gMutex;


struct BufferedQueue
{
    enum { Capacity = 100 };

    BufferedQueue() : mQuit(false)
    {
        (void)mPadding;
        mBuffer.reserve(Capacity);
    }

    ~BufferedQueue()
    {
        stop();
    }

    bool stoppped() const
    {
        return mQuit;
    }

    void stop()
    {
        if (!mQuit)
        {
            Lock lock(mMutex);
            mQuit = true;
            mCondition.notify_all();
        }
    }

    void push(int n)
    {
        Lock lock(mMutex);
        mBuffer.push_back(n);
        if (mBuffer.size() == mBuffer.capacity())
        {
            mQueue.push_back(std::move(mBuffer));
            mBuffer.clear();
            mBuffer.reserve(Capacity);
            mCondition.notify_all();
        }
    }

    std::vector<int> pop(std::chrono::nanoseconds timeout)
    {
        Buffer swap_buffer;
        swap_buffer.reserve(Capacity);
        auto start_time = Clock::now();

        for (;;)
        {

            // first check the queue
            {
                Lock lock(mMutex);
                mCondition.wait_until(lock, start_time + timeout);

                // we have a result
                if (!mQueue.empty())
                {
                    swap_buffer.swap(mQueue.front());
                    mQueue.pop_front();
                    return swap_buffer;
                }

            }

            // check if this was an early wakeup (spurious wakeup)
            // if yes, then we can resume waiting
            if (Clock::now() - start_time < timeout)
            {
                continue;
            }

            // timeout has occurred.
            // queue was empty, but perhaps we have some data in the buffer
            // that we can return
            {
                Lock lock(mMutex);


                if (!mBuffer.empty())
                {
                    // yes! we got some data
                    // swap the buffer with our local one
                    // and return
                    swap_buffer.swap(mBuffer);
                    return swap_buffer;
                }
            }

            // we got nothing but timeout has occured.
            // return empty result to user
            return Buffer();
        }
    }

    std::vector<int> pop()
    {
        Buffer swap_buffer;
        swap_buffer.reserve(Capacity);

        for (;;)
        {
            Lock lock(mMutex);
            mCondition.wait(lock);

            // we have a result
            if (!mQueue.empty())
            {
                swap_buffer.swap(mQueue.front());
                mQueue.pop_front();
                return swap_buffer;
            }

            if (!mBuffer.empty())
            {
                // yes! we got some data
                // swap the buffer with our local one
                // and return
                swap_buffer.swap(mBuffer);
                return swap_buffer;
            }

            if (mQuit)
            {
                return Buffer();
            }
        }
    }

private:
    typedef std::unique_lock<std::mutex> Lock;
    typedef std::vector<int> Buffer;

    std::atomic<bool> mQuit;
    Buffer mBuffer;
    std::list<Buffer> mQueue;
    char mPadding[64];
    std::condition_variable mCondition;
    std::mutex mMutex;
};


using namespace std::chrono;


int main()
{
    BufferedQueue queue;

    const auto max = 12345;

    std::thread producer([&]{
        for (int i = 0; i != max; ++i) {
            queue.push(i);
        }
    });

    std::thread consumer([&]{
        auto count = 0;
        for (;;) {
            auto result = queue.pop(std::chrono::microseconds(100));
            if (!result.empty()) {
                count += result.size();
                std::cout << result.size() << ": " << count << "/" << max << std::endl;
            }

            if (queue.stoppped()) {
                return;
            }

            if (count == max) {
                std::cout << "\nDone!" << std::endl;
                return;
            }
        }
    });

    std::thread([&]{
        std::this_thread::sleep_for(seconds(2));
        std::cout << "Stopping the queue" << std::endl;
        queue.stop();
    }).detach();

    producer.join();
    consumer.join();
}
