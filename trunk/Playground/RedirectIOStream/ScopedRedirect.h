#ifndef SCOPEDREDIRECT_H
#define SCOPEDREDIRECT_H


#include <ostream>


class ScopedRedirect
{
public:
    ScopedRedirect(std::ostream & inFrom, std::ostream & inTo);

    ~ScopedRedirect();

private:
    ScopedRedirect(const ScopedRedirect&);
    ScopedRedirect& operator=(const ScopedRedirect&);

    struct Impl;
    Impl * mImpl;
};


#endif // SCOPEDREDIRECT_H
