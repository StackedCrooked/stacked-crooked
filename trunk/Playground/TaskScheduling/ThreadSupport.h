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


template<typename F>
auto MakePackagedTask(F f) -> std::packaged_task<decltype(f())()>
{
    return std::packaged_task<decltype(f())()>(f);
}


template<typename F>
auto Async(F f) -> std::future<decltype(f())>
{
    auto task = MakePackagedTask(f);
    auto future_result = task.get_future();
    std::thread(std::move(task)).detach();
    return future_result;
}


} // namespace TreadSupport
