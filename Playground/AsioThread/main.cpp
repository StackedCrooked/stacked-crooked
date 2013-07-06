#include "tbb/concurrent_queue.h"
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>

namespace {

using Clock = std::chrono::high_resolution_clock;

template<typename T>
using SharedPromise = std::shared_ptr<std::promise<T>>;

template<typename F>
auto make_shared_promise(F f) -> SharedPromise<decltype(f())>
{
    return std::make_shared<std::promise<decltype(f())>>();
}


struct Lifetime
{
    Lifetime() : mSharedPtr(nullptr, [](void*){}) {}

    void reset()
    {
        mSharedPtr.reset();
    }

    std::weak_ptr<void> get_lifetime_checker() const
    {
        return mSharedPtr;
    }

    std::shared_ptr<void> mSharedPtr;
};


struct Stack
{
    Stack() :
        mQuitFlag(false),
        mThread([=]{
            try {
                for (;;) {
                    std::function<void()> f;
                    this->q.pop(f);
                    f();
                }
            } catch (int) {
                std::cout << "Stopping the thread!" << std::endl;
            }
        })
    {
    }

    ~Stack()
    {
        mQuitFlag = true;
        q.push([]{throw 1;});
        mCondition.notify_all();
        mThread.join();
    }

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    template<typename F>
    auto dispatch(F f) -> std::future<decltype(f())>
    {
        auto p = make_shared_promise(f);
        q.push([=]{
            set_promise(*p, f);
        });
        return p->get_future();
    }

    template<typename F>
    auto schedule(F f, unsigned us) -> std::future<decltype(f())>
    {
        using namespace std::chrono;

        //auto lifetime_checker = mLifetime.get_lifetime_checker();
        auto absolute_time = Clock::now() + microseconds(us);

        std::packaged_task<void()> task([=]() mutable -> decltype(f()) {
            if (mQuitFlag) {
                throw std::runtime_error("QuitFlag is set.");
            }
            std::unique_lock<std::mutex> lock(mMutex);
            while (Clock::now() < absolute_time) {
                mCondition.wait_until(lock, absolute_time);
                if (mQuitFlag) {
                    throw std::runtime_error("QuitFlag is set.");
                }
            }
            return dispatch(f).get();
        });

        auto future = task.get_future();

        for (;;)
        {
            try
            {
                std::thread(std::move(task)).detach();
                return future;
            }
            catch (std::system_error& e)
            {
                std::cout << e.what() << std::endl;
                std::unique_lock<std::mutex> lock(mMutex);
                mCondition.wait(lock);
            }
        }

    }

    template<typename R, typename F>
    void set_promise(std::promise<R>& p, F f)
    {
        p.set_value(f());
    }

    template<typename F>
    void set_promise(std::promise<void>& p, F f)
    {
        f();
        p.set_value();
    }

    tbb::concurrent_bounded_queue<std::function<void()>> q;
    std::atomic<bool> mQuitFlag;
    std::condition_variable mCondition;
    mutable std::mutex mMutex;
    std::thread mThread;
};

}


int main()
{
    std::cout << __VERSION__ << std::endl;
    std::thread([]{ sleep(2); std::cout << "Timeout!" << std::endl; std::abort(); }).detach();
    Stack s;
    s.dispatch([]{});

    std::cout << s.dispatch([] { return "Hello dispatch!"; }).get() << std::endl;

    for (uint64_t i = 1; i != 1024*1024 ; i = std::max((3 * i) / 2, i + 1))
    {

        using namespace std::chrono;
        auto scheduled_time = Clock::now();
        s.schedule([=] {
            auto actual_executed_time = Clock::now() - scheduled_time;
            std::cout
                    << "intended_executed_time: " << i
                    << "\tactual_executed_time: " << duration_cast<microseconds>(actual_executed_time).count()
                    << "\toverhead: " << (actual_executed_time.count() - i)
                    << std::endl;
        }, i);
    }
    usleep(1000000);
}

