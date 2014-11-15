#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <new>
#include <utility>
#include <array>




template<typename T>
using Invoke = typename T::type;

template <typename Condition, typename T = void>
using DisableIf = Invoke<std::enable_if<!Condition::value, T>>;

template <typename T>
using RemoveCV = Invoke<std::remove_const<Invoke<std::remove_reference<T>>>>;

template <typename T, typename U>
struct IsRelated : std::is_same<RemoveCV<T>, RemoveCV<U>> {};




template<typename Signature>
struct Function;


template<typename R, typename ...Args>
struct Function<R(Args...)>
{
    // this constructor accepts lambda, function pointer or functor
    template<typename F, DisableIf<IsRelated<Function, F>>...>
    Function(F&& f) :
        mImpl(new Impl<F>(std::forward<F>(f)))
    {
    }

    // this constructor accepts lambda, function pointer or functor
    template<typename Alloc, typename F, DisableIf<IsRelated<Function, F>>...>
    Function(std::allocator_arg_t, Alloc alloc, F&& f) :
        mImpl(new (typename Alloc::template rebind<Impl<F>>::other(alloc).allocate(1)) Impl<F>(std::forward<F>(f)))
    {
    }

    Function(Function&&) noexcept = default;
    Function& operator=(Function&&) noexcept = default;

    Function(const Function&) noexcept = default;
    Function& operator=(const Function&) noexcept = default;

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

    auto copy = increment;
    std::cout << copy(3) << std::endl;

    Function<int(int)> inc(std::allocator_arg, std::allocator<Function<int(int)>>(), [=](int n) { return n + 1; });
    std::cout << inc(5) << std::endl;


}
