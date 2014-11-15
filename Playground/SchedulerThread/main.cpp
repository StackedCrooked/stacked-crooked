#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <boost/container/flat_set.hpp>
#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>
#define TRACE() std::cout << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << std::endl

class Internal
{
    friend class SchedulerThread;
    Internal() {}
};

//! Wrapper for std::thread which joins on destruction.
class Thread
{
public:
    template<typename F>
    Thread(F&& f) :
        mThread([=]{ try { f(); } catch (...) { } })
    {
    }

    ~Thread()
    {
        join();
    }

    bool joinable() const
    {
        return mThread.joinable();
    }

    void join()
    {
        if (mThread.joinable())
        {
            mThread.join();
        }
    }

private:
    std::thread mThread;
};


struct my_handler
{
    my_handler()
    {
        TRACE();
    }

    my_handler(const my_handler&)
    {
        TRACE();
    }

    my_handler& operator=(const my_handler&)
    {
        TRACE();
        return *this;
    }

    template<typename ...Args>
    void operator()(Args&& ...)
    {
        TRACE();
    }
};



    char data[uint16_t(-1)];
    uint32_t mUsed = 0;
    uint32_t mFreed = 0;


    enum
    {
        Capacity = std::numeric_limits<uint16_t>::max()
    };

    void* allocate(std::size_t n)
    {
        TRACE();
        if (n > Capacity)
        {
            return nullptr;
        }

        if (mUsed + n >= Capacity)
        {
            mUsed = 0;
        }

        void* result = data;
        mUsed += n;
        return result;
    }


    void deallocate(void* , std::size_t n)
    {
        TRACE();
        mFreed += n;
    }




void* asio_handler_allocate(std::size_t n, my_handler*)
{
    TRACE();
    return allocate(n);
}

void asio_handler_deallocate(void* pointer, std::size_t size, my_handler*)
{
    TRACE();
    deallocate(pointer, size);
}

boost::asio::io_service ios;

void test()
{
    TRACE();
    my_handler handler;
    ios.post(handler);
}


class SchedulerThread
{
public:
    using Clock = std::chrono::steady_clock;

    SchedulerThread() : mThread([=]{ this->schedulerThread(); }) {}

    ~SchedulerThread()
    {
        stop();
    }

    void stop()
    {

        if (!mThread.joinable())
        {
            return;
        }
        std::unique_lock<std::mutex> lock(mMutex);
        mTasks.clear();
        mTasks.insert(Task(Clock::time_point(), [](Internal){}));
        mCondition.notify_all();
        lock.unlock();
        mThread.join();
    }

    template<typename F>
    void post(F&& f)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mTasks.insert(Task(Clock::now(), std::forward<F>(f)));
        mCondition.notify_all();
    }

    template<typename F>
    void dispatch(F&& f)
    {
        if (try_lock())
        {
            std::lock_guard<std::mutex> lock(mMutex, std::adopt_lock);
            f(Internal());
            return;
        }

        std::lock_guard<std::mutex> lock(mMutex);
        mTasks.insert(Task(Clock::now(), std::forward<F>(f)));
        mCondition.notify_all();

    }
    void test_handle()
    {

    }

    template<typename F>
    void dispatch_async(F&& f)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mTasks.insert(Task(Clock::now(), std::forward<F>(f)));
        mCondition.notify_all();
    }

    template<typename F>
    void post_after(F&& f, Clock::duration delay)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mTasks.insert(Task(Clock::now() + delay, std::forward<F>(f)));
        mCondition.notify_all();
    }

    struct Lock
    {
        Lock(SchedulerThread& inSchedulerThread) :
            mLock(inSchedulerThread.mMutex)
        {
        }

        Lock(SchedulerThread& inSchedulerThread, std::adopt_lock_t) :
            mLock(inSchedulerThread.mMutex, std::adopt_lock)
        {
        }

        operator Internal() const { return Internal(); }

    private:
        std::lock_guard<std::mutex> mLock;
    };

    void lock()
    {
        mMutex.lock();
    }

    void unlock()
    {
        mMutex.lock();
    }

    bool try_lock()
    {
        return mMutex.try_lock();
    }


private:
    struct Task
    {
        template<typename F>
        Task(Clock::time_point inFutureTime, F&& inFunction) :
            mFutureTime(inFutureTime),
            mFunction(std::move(inFunction))
        {
        }

        Task() : mFutureTime(), mFunction()
        {
        }

        bool ready_to_run(Clock::time_point current_time) const
        {
            return mFutureTime <= current_time;
        }

        void operator()(Internal in) const
        {
            mFunction(in);
        }

        friend bool operator<(const Task& lhs, const Task& rhs)
        {
            return lhs.mFutureTime < rhs.mFutureTime;
        }

        Clock::time_point mFutureTime;
        mutable std::function<void(Internal)> mFunction;
    };

    void schedulerThread()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        for (;;)
        {
            while (mTasks.empty())
            {
                mCondition.wait(lock);
            }

            while (!mTasks.empty())
            {
                const Task& task = *mTasks.begin();
                if (task.mFutureTime == Clock::time_point())
                {
                    mTasks.clear();
                    return; // this ends the thread
                }
                else if (task.ready_to_run(Clock::now()))
                {
                    task.mFunction(Internal());
                    mTasks.erase(mTasks.begin());
                }
                else
                {
                    mCondition.wait_until(lock, task.mFutureTime);
                }
            }
        }
    }

private:
    mutable boost::container::flat_multiset<Task> mTasks;
    mutable std::condition_variable mCondition;
    mutable std::mutex mMutex;
    Thread mThread;
};




void run()
{
    SchedulerThread scheduler;
    scheduler.post([=](Internal) { std::cout << "Posted " << __LINE__ << std::endl; });
    scheduler.post([=](Internal) { std::cout << "Posted " << __LINE__ << std::endl; });
    scheduler.dispatch([=](Internal) { std::cout << "dispatched " << __LINE__ << std::endl; });
    scheduler.dispatch([=](Internal) { std::cout << "dispatched " << __LINE__ << std::endl; });
    scheduler.post([=](Internal) { std::cout << "Posted " << __LINE__ << std::endl; });
    scheduler.post([=](Internal) { std::cout << "Posted " << __LINE__ << std::endl; });
    scheduler.dispatch([=](Internal) { std::cout << "dispatched " << __LINE__ << std::endl; });
    scheduler.dispatch([=](Internal) { std::cout << "dispatched " << __LINE__ << std::endl; });
}

int main()
{
    test();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "End of program." << std::endl;
}
