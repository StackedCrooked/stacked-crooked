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
#include <time.h>


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

    void post(int64_t n, bool /*flush*/)
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
        mSharedQueue.reserve(1);
        mProducerQueue.reserve(1);
        mConsumerQueue.reserve(1);
    }

    ~Queue()
    {
        std::cout << "mSharedQueue.capacity=" << mSharedQueue.capacity() << std::endl;
        std::cout << "mProducerQueue.capacity=" << mProducerQueue.capacity() << std::endl;
        std::cout << "mConsumerQueue.capacity=" << mConsumerQueue.capacity() << std::endl;
    }

    void post(int64_t n, bool flush)
    {
        mProducerQueue.push_back(n);

        if (!flush)
        {
            return;
        }
        
        bool was_empty = false;

        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (mSharedQueue.empty())
            {
                was_empty = true;
                std::swap(mProducerQueue, mSharedQueue);
            }
            else
            {
                mSharedQueue.insert(
                    mSharedQueue.end(),
                    mProducerQueue.begin(),
                    mProducerQueue.end());
                mProducerQueue.clear();
            }
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
            std::unique_lock<std::mutex> lock(mMutex); 
            while (mSharedQueue.empty())
            {
                mCondition.wait(lock); // unlocks the mutex
            }
            std::swap(mSharedQueue, mConsumerQueue);
        }

        for (auto n : mConsumerQueue)
        {
            f(n);
        }
        mConsumerQueue.clear();
    }


    __attribute__((aligned(64))) std::vector<int64_t> mProducerQueue;
    __attribute__((aligned(64))) std::vector<int64_t> mSharedQueue;
    __attribute__((aligned(64))) std::vector<int64_t> mConsumerQueue;
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
    for (int64_t i = 1; i <= num_iterations; ++i)
    {
        queue.post(i, i % 8 == 0);
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
