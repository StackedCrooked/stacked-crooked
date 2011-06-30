#ifndef SCOPEDMUTER_H
#define SCOPEDMUTER_H


#include <ostream>


class ScopedMuter
{
public:
    ScopedMuter(std::ostream & inOutStream);

    ~ScopedMuter();

private:
    ScopedMuter(const ScopedMuter&);
    ScopedMuter& operator=(const ScopedMuter&);

    struct FakeSink;

    struct Impl;
    Impl * mImpl;
};


#endif // SCOPEDMUTER_H
