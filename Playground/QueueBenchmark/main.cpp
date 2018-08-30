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


std::atomic<int64_t> counter{0};


#ifdef SEMAPHORE



struct Semaphore
{
    Semaphore()
    {
        if (-1 == sem_init(&mSem, 0, 0))
        {
            throw std::system_error(std::error_code(errno, std::system_category()), strerror(errno));
        }
    }

    ~Semaphore()
    {
        if (-1 == sem_destroy(&mSem))
        {
            // silently discard error
        }
    }

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    void post()
    {
        if (-1 == sem_post(&mSem))
        {
            throw std::system_error(std::error_code(errno, std::system_category()), strerror(errno));
        }
    }

    void wait()
    {
        if (-1 == sem_wait(&mSem))
        {
            throw std::system_error(std::error_code(errno, std::system_category()), strerror(errno));
        }
    }

private:
    mutable sem_t mSem;
};

struct Queue // using semaphore
{
    Queue() : mSem(), mQueue()
    {
        sched_yield(); // hack: wait for consumer_thread to start
    }

    ~Queue()
    {
        while (!mQueue.push(-1)) { sched_yield(); }
        mSem.post();
    }

    void post(int64_t n)
    {
        while (!mQueue.push(n)) { sched_yield(); }
        mSem.post();
    }

    template<typename F>
    void consume(F&& f)
    {
        for (;;)
        {
            // swap the buffers
            {
                std::unique_lock<std::mutex> lock(mMutex); 
                while (mEvents.empty())
                {
                    mCondition.wait(lock); // unlocks the mutex
                }
                std::swap(mEvents, mEvents2);
            }

            f(mEvents2);
            mEvents2.clear();
        }
    }

    void consumer_thread()
    {
        for (;;)
        {
            mSem.wait();
            int64_t n;
            mQueue.pop(n);
        }
    }

    Semaphore mSem;
    boost::lockfree::spsc_queue<int64_t, boost::lockfree::capacity<1000>> mQueue;
};


#else


struct Queue
{
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

        f(mEvents2);
        mEvents2.clear();
    }


    __attribute__((aligned(64))) std::vector<int64_t> mEvents;
    __attribute__((aligned(64))) std::vector<int64_t> mEvents2;
    std::condition_variable mCondition;
    std::mutex mMutex;
};

#endif

void test_queue()
{
    Queue queue;

    std::atomic<bool> consumer_started{false};
    std::atomic<bool> consumer_stopped{false};

    std::thread consumer_thread([&]{
        consumer_started = true;
        int num_consumed = 0;
        for (;;) {
            //std::cout << __FILE__ << ":" << __LINE__ << ": Consume" << std::endl;
            queue.consume([&](const std::vector<int64_t>& vec) {
                for (auto n : vec) {
                    counter += n;
                }
                //std::cout << __FILE__ << ":" << __LINE__ << ": Consumed vec.size=" << vec.size() << " counter=" << counter << " num_consumed=" << num_consumed << "/" << num_iterations << std::endl;
                num_consumed += vec.size();
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
    std::cout << "elapsed_s=" << elapsed_s << " rate=" << rate << " million events per second" << std::endl;

    consumer_thread.join();
}


int main()
{
    test_queue();
}
