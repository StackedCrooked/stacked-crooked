#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <new>
#include <utility>
#include <array>



template<typename Signature>
struct Function;


template<typename R, typename ...Args>
struct Function<R(Args...)>
{
    template<typename F>
    Function(F&& f) : mImpl(new Impl<F>(std::forward<F>(f))) { }

    R operator()(Args ...args)
    {
        return (*mImpl)(args...);
    }

    struct ImplBase
    {
        virtual ~ImplBase() {}

        virtual R operator()(Args ...args) = 0;

    };

    template<typename F>
    struct Impl : ImplBase
    {
        Impl(F&& f) : f(std::forward<F>(f)) {}
        virtual R operator()(Args ...args)
        {
            return f(args...);
        }

        F f;
    };

    std::shared_ptr<ImplBase> mImpl;
};



int main()
{
    Function<int(int)> increment([=](int n) { return n + 1; });
    std::cout << increment(3) << std::endl;
}
