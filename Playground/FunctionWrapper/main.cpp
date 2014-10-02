#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <atomic>
#include <deque>
#include <vector>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include "tbb/concurrent_queue.h"
#include <new>
#include <utility>
#include <array>


#include <cxxabi.h>


std::string Demangle(char const * name)
{
    int st;
    char * const p = abi::__cxa_demangle(name, 0, 0, &st);
    switch (st)
    {
        case 0: { return std::unique_ptr<char, decltype(&std::free)>(p, &std::free).get(); }
        case -1: throw std::runtime_error("A memory allocation failure occurred.");
        case -2: throw std::runtime_error("Not a valid name under the GCC C++ ABI mangling rules.");
        case -3: throw std::runtime_error("One of the arguments is invalid.");
        default: throw std::runtime_error("unexpected demangle status");
    }
}

template<typename Signature>
struct Function;


template<int> struct Size;

template<typename R, typename ...Args>
struct Function<R(Args...)>
{
    typedef Function<R(Args...)> this_type;

    Function() = delete;

    template<typename F>
    Function(const F& f) noexcept
    {
        static_assert(!std::is_same<F, this_type>::value, "");
        static_assert(sizeof(Impl<F>) <= sizeof(storage), "");

        static_assert(!std::is_reference<F>::value, "");
        static_assert(alignof(F) <= alignof(this_type), "");

        new (&storage) Impl<F>(f);
    }

    Function(Function&& rhs) = delete;
    Function(const Function& rhs)  = delete;

    Function& operator=(const Function& rhs) = delete;
    Function& operator=(Function&& rhs) = delete;

    ~Function()
    {
        base().~Base();
    }

    explicit operator bool() const
    {
        return valid();
    }

    R operator()(Args&& ...args) const
    {
        return base().call(std::forward<Args>(args)...);
    }

private:
    struct Base
    {
        virtual ~Base() {}
        virtual R call(const Args& ...args) const= 0;
    };


    template<typename F>
    struct Impl : Base
    {
        Impl(const F& f) : f(f) {}

        virtual R call(const Args& ...args) const override final
        { return f(args...); }

        F f;
    };

    void unset()
    {
        if (!valid())
        {
            return;
        }
        base().~Base();
        storage = Storage();
    }

    // convenience methods
    bool valid() const
    { return storage != Storage(); }

//    void mark_deleted()
//    {
//        for (auto&& i : storage)
//        {
//            static_assert(std::is_unsigned<Storage::value_type>::value, "");
//            i = Storage::value_type(-1);
//        }
//    }

//    bool is_deleted() const
//    {
//        for (auto&& i : storage)
//        {
//            if (i != Storage::value_type(-1))
//            {
//                return false;
//            }
//        }
//        return true;
//    }

    const void* data() const
    { return static_cast<const void*>(&storage); }

    void* data()
    { return static_cast<void*>(&storage); }

    const Base& base() const
    { assert(valid()); return *static_cast<const Base*>(data()); }

    Base& base()
    { assert(valid()); return *static_cast<Base*>(data()); }

    typedef std::array<uint64_t, 2> Storage;
    alignas(alignof(max_align_t)) Storage storage;
};



#include "tbb/concurrent_queue.h"
#include <thread>


const int64_t num_iterations = 10000000;

using namespace std::chrono;

typedef steady_clock Clock;


int64_t total_sum = 0;



template<typename Task>
void test_fast()
{
    auto now = []() -> int64_t { return duration_cast<microseconds>(Clock::now().time_since_epoch()).count(); };

    int64_t sum = 0;
    std::deque<Task> queue;

    auto t1 = now();

    std::string s = "asdfasdfasdfasd";

    for (long i = 0; i != num_iterations; ++i)
    {
        s.back() = char(i);
        queue.emplace_back([&s]{ return s.size() + total_sum; });
    }

    auto t2 = now();

    for (auto&& task : queue) sum += task();

    auto t3 = now();

    std::cout << (t2 - t1) << "+" << (t3 - t2) << "=" << (t3 - t1) << '\t';
    total_sum += sum;





}


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
    std::thread([]{ sleep(1); for (int i = 0; i != 10; ++i) { std::cout << '[' << i << ']'; } }).detach();
    run();
    run();
    run();
    std::cout << std::endl << " total_sum=" << total_sum << std::endl;
}
