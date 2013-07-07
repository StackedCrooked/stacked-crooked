#include <future>
#include <thread>


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


} // namespace TreadSupport
