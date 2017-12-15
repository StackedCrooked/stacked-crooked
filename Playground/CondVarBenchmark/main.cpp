#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>


//
// Intention is to determine cost of calling condition.notify_one()
// under the following conditions:
//
// - The notify is "loaded", so it actually wakes up the other thread
// - Each thread is pinned to it's own dedicated CPU core. (Not possible on OS X)
//
// Maybe later:
// - Test with sibling hypercores
// - What if the notify is done under the lock?
// 



std::array<std::chrono::nanoseconds, 10000> global_results;
std::atomic<uint32_t> global_result_size{};
std::atomic<uint32_t> num_notifies{};
std::atomic<uint32_t> num_wakeups{};




using Clock = std::chrono::steady_clock;



struct Waiter
{
    Waiter() :
        mThread([this] { run(); })
    {
    }

    void run()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            mCondition.wait(lock, [this] { return mCounter > 0; });
            num_wakeups++;
            //std::cout << "C " << mCounter << std::endl;
            mCounter = 0;
        }
    }


    void increment()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        auto old_count = mCounter++;
        if (old_count == 0)
        {
            auto start_time = Clock::now();
            mCondition.notify_one();
            num_notifies++;
            auto result_index = global_result_size++;
            global_results[result_index] = Clock::now() - start_time;
            if (result_index + 1 == global_results.size())
            {
                for (auto& result : global_results)
                {
                    std::cout << result.count() << std::endl;
                }
                std::cout << "num_notifies=" << num_notifies.load() << " num_wakeups=" << num_wakeups.load() << std::endl;
                std::exit(0);
            }
        }
    }

    uint64_t mCounter = 0; 
    uint64_t mWakeups = 0; 
    std::mutex mMutex;
    std::condition_variable mCondition;
    std::thread mThread;
};



struct Notifier
{
    void run()
    {
        for (;;)
        {
            waiter.increment();
        }
    }

    Waiter waiter;
};


int main()
{
    Notifier notifier;
    notifier.run();

}
