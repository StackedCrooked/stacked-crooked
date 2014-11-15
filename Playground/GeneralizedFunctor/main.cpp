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
    template<typename F, typename Enabler = DisableIf<IsRelated<F, Function>>>
    Function(F&& f) :
        mImpl(new Impl<F>(std::forward<F>(f)))
    {
    }

    // this constructor accepts lambda, function pointer or functor
    template<typename Alloc, typename F, typename Enabler = DisableIf<IsRelated<F, Function>>>
    Function(std::allocator_arg_t, Alloc alloc, F&& f);

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


    // this constructor accepts lambda, function pointer or functor
    template<typename R, typename ...Args>
    template<typename Alloc, typename F, typename>
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
        mImpl.reset(new (other.allocate(1)) Impl<F>(std::forward<F>(f)), Deallocator{other});
    }

int main()
{
    Function<int(int)> increment([=](int n) { return n + 1; });
    std::cout << increment(3) << std::endl;

    auto copy = increment;
    std::cout << copy(3) << std::endl;


    Function<std::string(std::string)> aa(std::allocator_arg, std::allocator<char>(), [=](std::string s) { return s + "a"; });
    auto bb = aa;



}

