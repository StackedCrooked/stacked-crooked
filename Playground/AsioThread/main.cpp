#include <boost/asio.hpp>
#include <future>
#include <iostream>
#include <unistd.h>

    
template<typename T>
using SharedPromise = std::shared_ptr<std::promise<T>>;

template<typename F>
auto make_shared_promise(F f) -> SharedPromise<decltype(f())>
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

struct Stack
{
    Stack()
    {
        std::thread([&]{ for(;;) this->ios.poll_one(); }).detach();
    }
    
    ~Stack()
    {
        ios.stop();
    }
    
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;
    
    template<typename F>
    auto dispatch(F f) -> std::future<decltype(f())>
    {
        auto p = make_shared_promise(f);
        this->ios.dispatch([=]{
            set_promise(*p, f);
        });
        return p->get_future();
    }
    
    boost::asio::io_service ios;
};


int main()
{
    std::thread([]{ sleep(1); std::cout << "TIMEOUT!!" << std::endl;throw 1; }).detach();
    Stack s;
    s.dispatch([]{});
    
    std::future<int> future_n = s.dispatch([] { return 3; });
    std::cout << future_n.get() << std::endl;
}

