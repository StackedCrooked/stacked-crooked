#include "FunctorImpl.h"

FunctorImpl::FunctorImpl() :
    Functor([=](int a, int b) { return a + b; })
{
}

Functor* CreateFunctor()
{
    return new FunctorImpl();
}
