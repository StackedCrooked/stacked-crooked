#include "ScopedRedirect.h"
#include <ostream>


struct ScopedRedirect::Impl
{
    Impl(std::ostream & inOriginal, std::ostream & inRedirect) :
        mOriginal(inOriginal),
        mRedirect(inRedirect)
    {
        mOriginal.rdbuf(mRedirect.rdbuf(mOriginal.rdbuf()));
    }

    ~Impl()
    {
        mOriginal.rdbuf(mRedirect.rdbuf(mOriginal.rdbuf()));
    }

    std::ostream & mOriginal;
    std::ostream & mRedirect;
};


ScopedRedirect::ScopedRedirect(std::ostream & inOriginal, std::ostream & inRedirect) :
    mImpl(new Impl(inOriginal, inRedirect))
{
}


ScopedRedirect::~ScopedRedirect()
{
    delete mImpl;
}

