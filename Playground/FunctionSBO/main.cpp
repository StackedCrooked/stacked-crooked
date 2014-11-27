#include <boost/bind.hpp>
#include <cassert>
#include <type_traits>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iostream>
#include <new>
#include <utility>
#include <array>


#define TRACE /*std::cout << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << std::endl*/;



template<typename T>
using Invoke = typename T::type;

template <typename Condition, typename T = void>
using EnableIf = Invoke<std::enable_if<Condition::value, T>>;

template <typename Condition, typename T = void>
using DisableIf = Invoke<std::enable_if<!Condition::value, T>>;

template <typename T>
using Decay = Invoke<std::remove_const<Invoke<std::remove_reference<T>>>>;

template <typename T, typename U>
struct IsRelated : std::is_same<Decay<T>, Decay<U>> {};


// restriction: only allow alignment up to alignment of pointer types. (so "long double" won't work)

template<int Size>
struct WithSize
{
    // forward declaration
    template<typename Signature>
    struct Function;

    template<typename R, typename ...Args>
    struct Function<R(Args...)>
    {
        Function()
        {
            setInvalid();
        }

        template<typename F, typename = DisableIf<IsRelated<F, Function>>>
        Function(F&& f)
        {
            TRACE
            using impl_type = Impl<Decay<F>, NoAlloc<Decay<F>>>;
            static_assert(alignof(impl_type) <= alignof(Storage), "");
            static_assert(sizeof(impl_type) <= sizeof(Storage), "");
            mStorage.setLocal(new (mStorage.getLocalStorage()) impl_type(std::forward<F>(f), NoAlloc<Decay<F>>{}));
        }

        template<typename Alloc, typename F, typename = DisableIf<IsRelated<F, Function>>>
        Function(std::allocator_arg_t, Alloc alloc, F&& f)
        {
            TRACE
            using impl_type = Impl<Decay<F>, Alloc>;
            using Other = typename Alloc::template rebind<impl_type>::other;

            static_assert(alignof(impl_type) <= alignof(Storage), "");
            static_assert(sizeof(impl_type) <= sizeof(Storage), "");

            Other other(alloc);
            mStorage.setAllocated(new (other.allocate(10)) impl_type(std::forward<F>(f), other));
        }

        Function(const Function& rhs)
        {
            TRACE
            if (auto where = rhs.base()->allocate_copy())
            {
                rhs.base()->copy_to(where);
                mStorage.setAllocated(where);
            }
            else
            {
                rhs.base()->copy_to(mStorage.getLocalStorage());
                mStorage.setLocal();
            }
        }

        Function& operator=(const Function& rhs)
        {
            TRACE
            if (this != &rhs)
            {
                base()->~Base();
                if (auto where = rhs.base()->allocate_copy())
                {
                    rhs.base()->copy_to(where);
                    mStorage.setAllocated(where);
                }
                else
                {
                    rhs.base()->copy_to(mStorage.getLocalStorage());
                    mStorage.setLocal();
                }
            }
            return *this;
        }

        Function(Function&& rhs) noexcept
        {
            TRACE
            mStorage = rhs.mStorage;
            rhs.setInvalid();
        }

        Function& operator=(Function&& rhs) noexcept
        {
            TRACE
            if (this != &rhs)
            {
                base()->~Base();
                mStorage = rhs.mStorage;
                rhs.setInvalid();
            }
            return *this;
        }

        ~Function()
        {
            TRACE
            base()->~Base();
        }

        R operator()(Args... args) const
        {
            TRACE
            base()->call(args...);
        }

    private:
        struct Base
        {
            virtual ~Base() {}
            virtual R call(Args ...args) const = 0;
            virtual void copy_to(void* where) const = 0;
            virtual void move_to(void* where) = 0;
            virtual void* allocate_copy()  = 0;
        };


        template<typename F>
        struct NoAlloc
        {
            void* allocate(std::size_t) { return nullptr; }
            void deallocate(void*, std::size_t) { }
        };


        template<typename F, class Alloc>
        struct Impl : Base, Alloc
        {
            template<typename FF>
            Impl(FF&& ff, Alloc alloc) : Alloc(alloc), f(std::forward<FF>(ff))
            {
                TRACE
            }

            void destroy()
            {
                TRACE
                Alloc alloc = *this;
                this->~Impl();
                alloc.deallocate(this);
            }

            virtual void* allocate_copy()
            {
                return Alloc::allocate(1);
            }


            R call(Args ...args) const override final
            { return f(args...); }

            void copy_to(void* where) const override final
            { new (where) Impl<F, Alloc>(f, *this); }

            void move_to(void* where) override final
            { new (where) Impl<F, Alloc>(std::move(f), *this); }

            F f;
        };

        void setInvalid()
        {
            auto f = [](Args...) -> R { throw std::bad_function_call(); };
            using F = Decay<decltype(f)>;

            typedef Impl<F, NoAlloc<F>> impl_type;

            new (mStorage.getLocalStorage()) impl_type(std::move(f), NoAlloc<F>{});
            mStorage.mController = +[](const Storage& storage) {
                return (Base*)(storage.getLocalStorage());
            };

        }


        typedef void* VoidPtr;
        typedef void(*VoidFPtr)(void);
        struct Storage;
        typedef Base*(*ControllerPtr)(const Storage&);


        Base* base() const
        {
            return mStorage.mController(mStorage);
        }

        struct Storage
        {
            typedef Base* BasePtr;

            friend bool operator==(const Storage& lhs, const Storage& rhs)
            { return !std::memcmp(lhs.data(), rhs.data(), sizeof(Storage)); }

            friend bool operator!=(const Storage& lhs, const Storage& rhs)
            { return !!std::memcmp(lhs.data(), rhs.data(), sizeof(Storage)); }

            const void* getLocalStorage() const { return &mPointers[0]; }
            void* getLocalStorage() { return &mPointers[0]; }

            const void* getAllocatedStorage() const { return mPointers[0]; }
            void* getAllocatedStorage() { return mPointers[0]; }

            void setAllocated(void* ptr)
            {
                mPointers[0] = ptr;
                mController = [](const Storage& storage) {
                    return (Base*)(storage.getAllocatedStorage());
                };
            }

            void setLocal(void* = nullptr)
            {
                mController = +[](const Storage& storage) {
                    return (Base*)(storage.getLocalStorage());
                };
            }

            ControllerPtr mController;
            VoidPtr mPointers[1 + Size / sizeof(void*)];
        };

        Storage mStorage;
    };

};


template<typename Signature> struct FunctionFactory;


template<typename R, typename ...Args>
struct FunctionFactory<R(Args...)>
{
private:
    template<typename F>
    struct Helper
    {
        enum
        {
            max_align = sizeof(void*),
            N = sizeof(F),
            align = alignof(F) > max_align ? alignof(F) : max_align,
            value = N % align == 0 ? N : align * (1 + N / align)
        };
    };


public:
    template<typename F>
    using function_type = typename WithSize<Helper<F>::value>::template Function<R(Args...)>;

    template<typename F>
    static function_type<F> create_from(F&& f)
    {
        return function_type<Decay<F>>(std::forward<F>(f));
    }

    template<typename Alloc, typename F>
    static function_type<F> create_from(std::allocator_arg_t, Alloc alloc, F&& f)
    {
        return function_type<Decay<F>>(std::allocator_arg, alloc, std::forward<F>(f));
    }
};


int main()
{

    std::string t = "t";
    auto test = FunctionFactory<void(std::string&)>::create_from(std::allocator_arg, std::allocator<int>(), [&](std::string& s) { s += t += "t"; });
    TRACE

    std::string s = "s";
    TRACE
    t = "1";
    TRACE

    test(s);
    TRACE
    std::cout << s << std::endl;
    auto copy = test;
    t = "3";
    copy(s);
    std::cout << s << std::endl;
    t = "4";
    copy(s);
    std::cout << s << std::endl;


    // the correct type of allocator is probably this one
    // store the lambda in a variable because we need it to define both the allocator type and the
    // eventual functor
    auto increment = FunctionFactory<int(int)>::create_from(std::allocator_arg, std::allocator<int>{}, [](int n) { return n + 1; });


    // however, we can use any allocator since it will be rebound anyway
    auto decrement = FunctionFactory<int(int)>::create_from(std::allocator_arg, std::allocator<int>{}, [](int n) { return n - 1; });


#undef assert
    #define assert(cmd) std::cout << #cmd << std::flush << ": " << ((cmd) ? "OK" : (std::abort(), "FAIL")) << std::endl;


    assert(increment(3) == 4);
    assert(decrement(3) == 2);

    auto a = increment;
    auto b = decrement;

    assert(a(3) == 4);
    assert(b(3) == 2);

    std::swap(a, b);

    assert(a(3) == 2);
    assert(b(3) == 4);

    a = b;

    assert(a(3) == 4);
    assert(b(3) == 4);

    b = a;

    assert(a(3) == 4);
    assert(b(3) == 4);


    b = std::move(a);

    assert(b(3) == 4);

    a = b;

    assert(a(3) == 4);
    assert(b(3) == 4);

    b = std::move(a);
    a = std::move(b);
    b = a;

    assert(a(3) == 4);
    assert(b(3) == 4);

    std::swap(increment, decrement);

    assert(increment(3) == 2);
    assert(decrement(3) == 4);

    auto increment_copy = increment;
    std::cout << "increment_copy=" << increment_copy(3) << std::endl;

    std::string s123 = "123";
    auto mix1 = FunctionFactory<int()>::create_from([=]{ return increment(3) + decrement(4) + s123.size();
    });
    auto mix2 = FunctionFactory<int()>::create_from([=] { return mix1() + increment(3) + decrement(4) + s123.size();
    });
    auto mix3 = FunctionFactory<int()>::create_from([=] { return mix2() + mix1() + increment(3) + decrement(4) + s123.size();
    });

    std::cout << "MIX1: " << mix1() << std::endl;
    std::cout << "MIX2: " << mix2() << std::endl;
    std::cout << "MIX3: " << mix3() << std::endl;

    std::cout
        << "sizeof(s123)=" << sizeof(s123)
        << "\nsizeof(increment)=" << sizeof(increment)
        << "\nsizeof(decrement)=" << sizeof(decrement)
        << "\nsizeof(mix1)=" << sizeof(mix1)
        << "\nsizeof(mix2)=" << sizeof(mix2)
        << "\nsizeof(mix3)=" << sizeof(mix3)
        << std::endl;

    auto steal_mix3 = std::move(mix3);
    std::cout << "MIX1: " << mix1() << std::endl;
    std::cout << "MIX2: " << mix2() << std::endl;
    std::cout << "STEAL_MIX3: " << steal_mix3() << std::endl;

    try { mix3();
    std::abort();
    } catch (std::bad_function_call&) {}

    std::cout << "End of program." << std::endl;
    auto stateless = FunctionFactory<int()>::create_from([] { return 1;
    });
    std::cout << sizeof(stateless) << " align=" << alignof(stateless) << std::endl;



}
