#include <future>
#include <thread>


namespace TreadSupport {


template<typename F>
auto make_shared_promise(F f) -> std::shared_ptr<std::promise<decltype(f())>>
{
    return std::make_shared<std::promise<decltype(f())>>();
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


} // namespace ThreadSupport
