#include <cstddef>
#include <utility>
#include <array>


template<typename Signature>
struct Function;


template<int> struct Size;

template<typename R, typename ...Args>
struct Function<R(Args...)>
{
    typedef Function<R(Args...)> this_type;

    Function() noexcept = delete;

    template<typename F>
    Function(const F& f) noexcept { new (&storage) Impl<F>(f); }

    Function(Function&& rhs) noexcept = delete;
    Function& operator=(Function&& rhs) noexcept = delete;

    Function(const Function& rhs) noexcept = delete;
    Function& operator=(const Function& rhs) noexcept = delete;

    ~Function() noexcept { base().~Base(); }

    R operator()(Args&& ...args) const noexcept
    {
        return base().call(std::forward<Args>(args)...);
    }

private:
    struct Base
    {
        virtual ~Base() noexcept {}
        virtual R call(const Args& ...args) const noexcept = 0;
    };

    template<typename F>
    struct Impl : Base
    {
        Impl(const F& f) noexcept  : f(f) {}

        virtual R call(const Args& ...args) const noexcept override final
        { return f(args...); }

        F f;
    };

    const void* data() const noexcept
    { return static_cast<const void*>(&storage); }

    const Base& base() const noexcept
    { return *static_cast<const Base*>(data()); }

    typedef std::array<uint64_t, 2> Storage;
    alignas(alignof(max_align_t)) Storage storage;
};



#include <deque>
#include <iostream>
#include <chrono>


const int64_t num_iterations = 2000000;
int64_t total_sum = 0;



template<typename Task>
void test_fast()
{
    auto now = []() -> int64_t { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); };

    int64_t sum = 0;
    std::deque<Task> queue;

    auto t1 = now();

    std::string s = "asdfasdfasdfasd";

    for (long i = 0; i != num_iterations; ++i)
    {
        s.back() = char(i);
        queue.emplace_back([&s]() noexcept { return s.size() + total_sum; });
    }

    auto t2 = now();

    for (auto&& task : queue) sum += task();

    auto t3 = now();

    std::cout << (t2 - t1) << "+" << (t3 - t2) << "=" << (t3 - t1) << '\t';
    total_sum += sum;
}


#include <functional>


void run()
{
    typedef Function<int()> MyFunction;
    std::cout << "MyFunction:  ";
    test_fast<MyFunction>();
    test_fast<MyFunction>();
    std::cout << std::endl;

    typedef std::function<int()> StdFunction;
    std::cout << "StdFunction: ";
    test_fast<StdFunction>();
    test_fast<StdFunction>();
    std::cout << std::endl;
}


int main()
{
    run();
    run();
    run();
    std::cout << std::endl << " total_sum=" << total_sum << std::endl;
}
