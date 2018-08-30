#ifndef SEMAPHORE
#error "SEMAPHORE not set"
#endif


#include <boost/lockfree/spsc_queue.hpp>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <math.h>
#include <mutex>
#include <system_error>
#include <thread>
#include <vector>
#include <semaphore.h>


enum { num_iterations = 10 * 1000 * 1000 };


#if SEMAPHORE == 1


struct Queue
{
    Queue() :
        mSemaphore(),
        mBuffer()
    {
        sem_init(&mSemaphore, 0, 0);
    }

    void post(int64_t n)
    {
        while (!mBuffer.push(n))
        {
            asm volatile ("pause;");
        }
        sem_post(&mSemaphore);
    }

    template<typename F>
    void consume(F&& f)
    {
        sem_wait(&mSemaphore);
        int64_t n = -1;
        if (!mBuffer.pop(n))
        {
            std::cerr << "Failed to pop from buffer!" << std::endl;
        }

        f(n);
    }

    sem_t mSemaphore;
    boost::lockfree::spsc_queue<int64_t, boost::lockfree::capacity<1000>> mBuffer;
};


#else


struct Queue
{
    Queue()
    {
        mEvents.reserve(1000);
        mEvents2.reserve(1000);
    }
    void post(int64_t n)
    {
        //std::cout << __FILE__ << ":" << __LINE__ << ": post(" << n << ")" << std::endl;
        bool was_empty = false;

        {
            std::lock_guard<std::mutex> lock(mMutex);
            was_empty = mEvents.empty();
            mEvents.push_back(n);
        }

        if (was_empty)
        {
            mCondition.notify_one();
        }
    }

    template<typename F>
    void consume(F&& f)
    {
        // swap the buffers
        {
            //std::cout << __FILE__ << ":" << __LINE__ << ": consume" << std::endl;
            std::unique_lock<std::mutex> lock(mMutex); 
            while (mEvents.empty())
            {
                //std::cout << __FILE__ << ":" << __LINE__ << ": consume" << std::endl;
                mCondition.wait(lock); // unlocks the mutex
            }
            std::swap(mEvents, mEvents2);
        }
        //std::cout << __FILE__ << ":" << __LINE__ << ": consume: mEvents2.size=" << mEvents2.size() << std::endl;

        for (auto n : mEvents2)
        {
            f(n);
        }
        mEvents2.clear();
    }


    __attribute__((aligned(64))) std::vector<int64_t> mEvents;
    __attribute__((aligned(64))) std::vector<int64_t> mEvents2;
    std::condition_variable mCondition;
    std::mutex mMutex;
};

#endif


void pin_to_core(int core_id)
{
    auto cpuset = cpu_set_t();
    CPU_SET(core_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
}

void test_queue(int core1, int core2)
{
	pin_to_core(core1);
    Queue queue;

    std::atomic<bool> consumer_started{false};
    std::atomic<bool> consumer_stopped{false};

    int64_t sum = 0;

    std::thread consumer_thread([&]{
		pin_to_core(core2);
        consumer_started = true;
        int num_consumed = 0;
        for (;;) {
            //std::cout << __FILE__ << ":" << __LINE__ << ": Consume" << std::endl;
            queue.consume([&](int64_t n) {
                num_consumed += 1;
                sum += n;
            });
            if (num_consumed  == num_iterations) {
                consumer_stopped = true;
                break;
            }
        }
    });

    //std::cout << "Waiting for consumer thread to start " << std::endl;
    while (!consumer_started) asm volatile ("pause;");

    auto t1 = std::chrono::system_clock::now();
    for (int64_t i = 0; i != num_iterations; ++i)
    {
        queue.post(i);
    }

    //std::cout << "Waiting for consumer thread to stop " << std::endl;
    while (!consumer_stopped) asm volatile ("pause;");
    auto t2 = std::chrono::system_clock::now();

    auto elapsed_ns = 1.0 * std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    auto elapsed_s = elapsed_ns / 1e9;
    auto rate = int64_t(0.5 + 10.0 * num_iterations / elapsed_s / 1000000.0)/10.0;
    std::cout << "elapsed_s=" << elapsed_s << " rate=" << rate << "M/s sum=" << sum << std::endl;

    consumer_thread.join();
}


int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " Core1 Core2" << std::endl;
        std::cerr << "\nExample: \n  " << argv[0] << " 1 2 " << std::endl;
        return 1;
    }


    auto core1 = std::strtoul(argv[1], NULL, 10);
    auto core2 = std::strtoul(argv[2], NULL, 10);
    std::cout << "Using cores: producer_core=" << core1 << " consume_core=" << core2 << std::endl;

    test_queue(core1, core2);
}
