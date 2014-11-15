#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>


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

    template<typename F>
    void dispatch_async(F&& f)
    {std::lock_guard<std::mutex> lock(mMutex);
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

        bool isQuitSignal() const
        {
            return mFutureTime == Clock::time_point();
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
    mutable std::multiset<Task> mTasks;
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
    run();
    std::cout << "End of program." << std::endl;
}
