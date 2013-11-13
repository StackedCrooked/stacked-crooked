#pragma once
#include <functional>

struct Functor
{
    template<typename F>
    Functor(F&& f) : f_(std::forward<F>(f)) {}

    int sum(int a, int b) const
    {
        return f_(a, b);
    }

    std::function<int(int, int)> f_;
};


Functor* CreateFunctor();
