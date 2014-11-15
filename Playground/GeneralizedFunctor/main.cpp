#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cassert>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <new>
#include <utility>
#include <array>


#define TRACE() std::cout << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << std::endl


template<typename T>
using Invoke = typename T::type;

template <typename Condition, typename T = void>
using EnableIf = Invoke<std::enable_if<Condition::value, T>>;

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
    template<typename F, DisableIf<IsRelated<F, Function>>...>
    Function(F&& f) : mImpl(new Impl<F>(std::forward<F>(f)))
    {
    }

    template<typename Alloc, typename F, DisableIf<IsRelated<F, Function>>...>
    Function(std::allocator_arg_t, Alloc alloc, F&& f);

    Function(Function&&) noexcept = default;
    Function& operator=(Function&&) noexcept = default;

    Function(const Function&) noexcept = default;
    Function& operator=(const Function&) noexcept = default;

    R operator()(Args ...args) const
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
        Impl(F&& f) : f(std::forward<F>(f)) { std::cout << "Impl::Impl()" << std::endl; }
        ~Impl() { std::cout << "Impl::~Impl" << std::endl; }

        virtual R operator()(Args ...args)
        {
            std::cout << "Impl::operator(...)" << std::endl;
            return f(args...);
        }

        F f;
    };

    std::shared_ptr<ImplBase> mImpl;
};


template<typename R, typename ...Args>
template<typename Alloc, typename F, DisableIf<IsRelated<F, Function<R(Args...)>>>...>
Function<R(Args...)>::Function(std::allocator_arg_t, Alloc alloc, F&& f)
{
    typedef typename Alloc::template rebind<Impl<F>>::other Other;
    Other other(alloc);

    struct Deallocator : private Other
    {
        Deallocator(Other& other) : Other(other) {}
        void operator()(Impl<F>* impl)
        {
            std::cout << "Deallocator::operator(Impl<F>*)" << std::endl;
            static_cast<Other&>(*this).deallocate(impl, 1);
        }
    };
    std::cout << "sizeof(Other) = " << sizeof(Other) << std::endl;
    std::cout << "sizeof(Deallocator) = " << sizeof(Deallocator) << std::endl;
    mImpl.reset(new (other.allocate(1)) Impl<F>(std::forward<F>(f)), Deallocator{other});
}


int main()
{
    Function<int(int)> increment([=](int n) { return n + 1; });
    assert(increment(3) == 4);

    Function<std::string(std::string, std::string)> concat(std::allocator_arg, std::allocator<int>(), [](std::string a, std::string b) { return a + b; });
    std::cout << concat("abc", "def") << std::endl;
}

