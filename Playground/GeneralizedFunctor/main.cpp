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




template<typename T>
using Invoke = typename T::type;

template <typename Condition, typename T = void>
using DisableIf = Invoke<std::enable_if<!Condition::value, T>>;

template <typename T>
using RemoveCV = Invoke<std::remove_const<Invoke<std::remove_reference<T>>>>;

template <typename T, typename U>
struct IsRelated : std::is_same<RemoveCV<T>, RemoveCV<U>> {};


#if 0
template <class _Alloc>
promise<void>::promise(allocator_arg_t, const _Alloc& __a0)
{
    typedef typename _Alloc::template rebind<__assoc_sub_state_alloc<_Alloc> >::other _A2;
    typedef __allocator_destructor<_A2> _D2;
    _A2 __a(__a0);
    unique_ptr<__assoc_sub_state_alloc<_Alloc>, _D2> __hold(__a.allocate(1), _D2(__a, 1));
    ::new(__hold.get()) __assoc_sub_state_alloc<_Alloc>(__a0);
    __state_ = __hold.release();
}
#endif

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
    Function(std::allocator_arg_t, Alloc alloc, F&& f)
    {
        struct ImplPlus : public Impl<F>, private Alloc
        {
            ImplPlus(F&& f, Alloc& alloc) : Impl<F>(std::forward<F>(f)), Alloc(alloc) {}

            using Impl<F>::operator ();

            void destroy()
            {
                Impl<F>& impl = *this;
                impl.~Impl<F>();

                Alloc& alloc = *this;
                typename Alloc::template rebind<ImplPlus>::other real_alloc(alloc);
                real_alloc.deallocate(this, 1);
            }
        };

        mImpl.reset(new (typename Alloc::template rebind<ImplPlus>::other(alloc).allocate(1)) ImplPlus(std::forward<F>(f), alloc), [](ImplPlus* impl) { impl->destroy(); });
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

    Function<std::string(std::string)> append(std::allocator_arg, std::allocator<char>(), [=](std::string s) { return s + "a"; });
    std::cout << append("abc") << std::endl;



}

