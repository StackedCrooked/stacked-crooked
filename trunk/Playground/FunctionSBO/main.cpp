#include <boost/bind.hpp>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iostream>
#include <new>
#include <utility>
#include <array>




template<int Size>
struct WithSize
{
    template<typename Signature>
    struct Function;
};



template<int Size>
template<typename R, typename ...Args>
struct WithSize<Size>::Function<R(Args...)>
{
    Function() : mStorage()
    {}

    template<typename F>
    Function(F f)
    {
        static_assert(alignof(Impl<F>) <= alignof(Storage), "");
        static_assert(sizeof(Impl<F>) <= sizeof(Storage), "");
        new (static_cast<void*>(&mStorage)) Impl<F>(f);
    }

    Function(const Function& rhs) :
        mStorage()
    {
        if (rhs.valid())
        {
            rhs.base().copy_to(data());
        }
    }

    Function(Function& rhs) :
        mStorage()
    {
        if (rhs.valid())
        {
            rhs.base().copy_to(data());
        }
    }

    Function(Function&& rhs) noexcept :
        mStorage(rhs.mStorage)
    {
        if (rhs.valid())
        {
            rhs.move_to(*this);
        }
    }

    Function& operator=(Function rhs) noexcept
    {
        if (valid())
        {
            base().~Base();
        }
        
        if (rhs.valid())
        {
            rhs.move_to(*this);
        }
        return *this;
    }

    ~Function()
    {
        if (valid())
        {
            base().~Base();
        }
    }

    explicit operator bool() const
    {
        return valid();
    }

    R operator()(Args&& ...args) const
    {
        if (!valid())
        {
            throw std::bad_function_call();
        }
        return base().call(std::forward<Args>(args)...);
    }

private:
    struct Base
    {
        virtual ~Base() {}
        virtual R call(Args&& ...args) const = 0;
        virtual void copy_to(void* where) const = 0;
        virtual void move_to(void* where) = 0;
    };


    template<typename F>
    struct Impl : Base
    {
        Impl(const F& f) : f(f) {}

        R call(Args&& ...args) const override final
        { return f(std::forward<Args>(args)...); }

        void copy_to(void* where) const override final
        { new (where) Impl<F>(*this); }


        void move_to(void* where) override final
        { new (where) Impl<F>(std::move(*this)); }

        F f;
    };

    void move_to(Function& dst)
    {
        assert(valid());
        if (dst.valid()) dst.base().~Base();
        base().move_to(dst.data());
        mStorage = Storage();
    }

    // convenience methods
    bool valid() const
    { return mStorage != Storage(); }

    const void* data() const
    { return static_cast<const void*>(mStorage.data()); }

    void* data()
    { return static_cast<void*>(mStorage.data()); }

    const Base& base() const
    { assert(valid()); return *static_cast<const Base*>(data()); }

    Base& base()
    { assert(valid()); return *static_cast<Base*>(data()); }


    struct Storage
    {
        Storage()
        {
            memset(data(), 0, sizeof(Storage));
        }

        const void* data() const
        { return static_cast<const void*>(&mStorage); }

        void* data()
        { return static_cast<void*>(&mStorage); }

        friend bool operator==(const Storage& lhs, const Storage& rhs)
        { return !std::memcmp(lhs.data(), rhs.data(), sizeof(Storage)); }

        friend bool operator!=(const Storage& lhs, const Storage& rhs)
        { return !!std::memcmp(lhs.data(), rhs.data(), sizeof(Storage)); }

        typedef void* max_align_t;
        max_align_t mStorage[1 + Size / sizeof(max_align_t)];
    };

    Storage mStorage;
};


template<typename Signature> struct FunctionFactory;




template<typename R, typename ...Args>
struct FunctionFactory<R(Args...)>
{
private:
    template<typename F>
    struct Size
    {
        enum
        {
            max_align_value = alignof(void*),
            N = sizeof(F),
            align = alignof(F) > max_align_value ? alignof(F) : max_align_value,
            value = N % align == 0 ? N : align * (1 + N / align)
        };
    };


public:
    template<typename F>
    using function_type = typename WithSize<Size<F>::value>::template Function<R(Args...)>;

    template<typename F>
    static function_type<F> create_from(F f)
    {
        return function_type<F>(std::move(f));
    }
};



int main()
{
    std::string t = "t";
    auto test = FunctionFactory<void(std::string&)>::create_from([&](std::string& s) { s += t += "t"; });
    
    std::string s = "s"; t = "1"; test(s); std::cout << s << std::endl;
    auto copy = test; t = "3"; copy(s); std::cout << s << std::endl;
    t = "4"; copy(s); std::cout << s << std::endl;
    
    
    auto increment = FunctionFactory<int(int)>::create_from([](int n) {
        return n + 1;
    });
    
    auto decrement = FunctionFactory<int(int)>::create_from([](int n) {
        return n - 1;
    });


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

    assert(!a);
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


    auto mix = FunctionFactory<void()>::create_from([=]{
        std::cout << increment(3) + decrement(4) << std::endl;
    });

    mix();

    std::cout << "sizeof(mix)= " << sizeof(mix)
        << " sizeof(increment)=" << sizeof(increment)
        << " sizeof(decrement)=" << sizeof(decrement)
        << std::endl;

    increment = std::move(decrement);
    std::cout << increment(3) << std::endl;


    // calling the moved-from decrement
    try {
        decrement(3);
        assert(false);
    } catch (std::bad_function_call& ) {
        std::cout << "OK: got bad_function_call as exptected." << std::endl;
    }




}
