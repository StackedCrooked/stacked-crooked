#include "tbb/concurrent_queue.h"
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
    Stack() :
        mQuit(std::make_shared<std::atomic<bool>>(false)),
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
        *mQuit = true;
        q.push([]{throw 1;});
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
    auto schedule(F f, unsigned ms) -> std::future<decltype(f())>
    {
        // start thread
        auto p = make_shared_promise(f);
        std::thread([=]{
            usleep(1000 * ms);
            if (*mQuit) return;
            set_promise(*p, [=]{ return dispatch(f).get(); });
        }).detach();
        return p->get_future();
    }
    
    std::shared_ptr<std::atomic<bool>> mQuit;
    tbb::concurrent_bounded_queue<std::function<void()>> q;
    std::thread mThread;
};


int main()
{
    std::thread([]{ sleep(5); std::cout << "Timeout!" << std::endl; std::abort(); }).detach();
    Stack s;
    s.dispatch([]{});

    std::cout << s.dispatch([] { return "Hello dispatch!"; }).get() << std::endl;

    s.schedule([] { std::cout << "1ms" << std::endl; }, 1);
    s.schedule([] { std::cout << "2ms" << std::endl; }, 2);
    s.schedule([] { std::cout << "3ms" << std::endl; }, 3).wait();
}

