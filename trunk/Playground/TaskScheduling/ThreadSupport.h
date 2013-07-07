#include "tbb/concurrent_queue.h"
#include <future>
#include <thread>
#include <unistd.h>


namespace ThreadSupport {


template<typename F>
auto MakeSharedPromise(F f) -> std::shared_ptr<std::promise<decltype(f())>>
{
    return std::make_shared<std::promise<decltype(f())>>();
}


template<typename R, typename F>
void SetPromise(std::promise<R>& p, F f)
{
    p.set_value(f());
}


template<typename F>
void SetPromise(std::promise<void>& p, F f)
{
    f();
    p.set_value();
}


template<typename F, typename ...Args>
auto Async(F f, Args&& ...args) -> std::future<decltype(f(std::declval<Args>()...))>
{
    typedef decltype(f(std::declval<Args>()...)) R;
    auto task = std::packaged_task<R(Args...)>(f);
    std::future<R> future = task.get_future();
    std::thread(std::move(task), std::forward<Args>(args)...).detach();
    return future;
}


struct Scheduler
{
    Scheduler() : mLifetime(nullptr, [](void*){}) {}

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    template<typename F>
    auto dispatch(F f) -> std::future<decltype(f())>
    {
        auto p = MakeSharedPromise(f);
        q.push([=]{
            SetPromise(*p, f);
        });
        return p->get_future();
    }

    template<typename F>
    auto schedule(F f, int timeout) -> decltype(Async(f))
    {
        auto checker = get_checker();
        return Async([=] { this->do_schedule(f, timeout, checker); });
    }

private:
    template<typename F>
    auto do_schedule(F f, int timeout, std::weak_ptr<void> lifetime_checker) -> decltype(Async(f))
    {
        typedef std::chrono::system_clock Clock;
        auto absolute_time = Clock::now() + std::chrono::milliseconds(timeout);
        while (Clock::now() < absolute_time) {
            usleep(500);
            if (lifetime_checker.expired()) {
                throw std::runtime_error("cancelled");
            }
        }
        dispatch(f);
    }

    std::weak_ptr<void> get_checker() const
    {
        return mLifetime;
    }

    tbb::concurrent_bounded_queue<std::function<void()>> q;
    std::shared_ptr<void> mLifetime;
};

} // namespace TreadSupport
