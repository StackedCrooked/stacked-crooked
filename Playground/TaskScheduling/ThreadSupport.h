#include <future>
#include <thread>


namespace TreadSupport {


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


template<typename SinkFunction>
struct Dispatcher
{
    Dispatcher(SinkFunction sink_function) :
        sink_function_(sink_function)
    {
    }

    template<typename F>
    auto operator()(F f) const -> std::future<decltype(f())>
    {
        auto p = MakeSharedPromise(f);
        sink_function_([=]{ SetPromise(*p, f); });
        return p->get_future();
    }

    SinkFunction sink_function_;
};



template<typename SinkFunction>
Dispatcher<SinkFunction> MakeSink(SinkFunction sink)
{
    return Dispatcher<SinkFunction>(sink);
}


} // namespace ThreadSupport
