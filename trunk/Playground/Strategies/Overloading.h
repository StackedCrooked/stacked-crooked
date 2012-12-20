#ifndef Overloading_H
#define Overloading_H


#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <type_traits>


namespace Overloading {


enum Dimension {
    Width,
    Height
};

enum Orientation {
    Horizontal,
    Vertical
};



template<Dimension dim, Orientation orient> struct calculate_width;

enum BoxSizeAlgoritm {
    Accumulate,
    MaxSize
};

template<BoxSizeAlgoritm> struct calculate_size;

template<> struct calculate_size<BoxSizeAlgoritm::Accumulate>
{
    template<typename Container>
    void operator()(const Container & container) const
    {
        return std::accumulate(container.begin(), container.end(), 0);
    }
};

template<> struct calculate_size<BoxSizeAlgoritm::MaxSize>
{
    template<typename Container>
    void operator()(const Container & container) const
    {
        return std::accumulate(container.begin(), container.end(), 0);
    }
};

template<> struct calculate_dimension : calculate_size<GetAlgorithm<dim, orient>>
{
    template<typename Container>
    void operator()(const Container & container) const
    {
        return std::accumulate(container.begin(), container.end(), 0);
    }
};

template<> struct calculate_dimension<Orientation::Vertical>
{
    template<typename Container>
    void operator()(const Container & container) const
    {
        return std::max_element(container.begin(), container.end());
    }
};


struct VerticalBox
{
    template<typename Container>
    void calculate_width(const Container & container) const
    {
        return std::max_element(container.begin(), container.end());
    }
};


template<Orientation> struct Orientation2BoxType;
template<> struct Orientation2BoxType<Orientation::Horizontal> {  typedef HorizontalBox Type; };
template<> struct Orientation2BoxType<Orientation::Vertical  > {  typedef VerticalBox   Type; };

template<> struct Orientation2BoxType<Orientation::Horizontal>
{
    typedef VerticalBox Type;
};


template<Orientation orient>
struct BoxLayout
{
    int width() const
    {
        return calculate_width<orient>()(elements);
    }

    std::vector<int> elements;
};


template<typename void sum

//
// .cpp file
//
class NativeMutex
{
public:
    // factory method
    static std::unique_ptr<NativeMutex> Create();

    virtual ~NativeMutex() {}

    void lock()   { do_lock();   }

    void unlock() { do_unlock(); }

private:
    virtual void do_lock() = 0;
    virtual void do_unlock() = 0;

    pthread_mutex_t mMutex;
};


Mutex::Mutex() : mNativeMutex(NativeMutex::Create())
{
}


void Mutex::lock()
{
    mNativeMutex->lock();
}


void Mutex::unlock()
{
    mNativeMutex->unlock();
}

} // namespace Overloading


#include <pthread.h>


namespace Overloading {


class PosixMutex : public NativeMutex
{
public:
    PosixMutex() : NativeMutex(), mMutex()
    {
        pthread_mutex_init(&mMutex, NULL);
    }

    ~PosixMutex()
    {
        pthread_mutex_destroy(&mMutex);
    }

private:
    void do_lock()
    {
        pthread_mutex_lock(&mMutex);
    }

    void do_unlock()
    {
        pthread_mutex_lock(&mMutex);
    }

    pthread_mutex_t mMutex;
};



std::unique_ptr<NativeMutex> NativeMutex::Create()
{
    // ifdef posix
    return std::unique_ptr<NativeMutex>(new PosixMutex);
    // else: return type according to platform
}


class ScopedLock
{
public:
    ScopedLock(Mutex & inMutex) : mMutex(inMutex) {}

private:
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;

    Mutex & mMutex;
};


void test()
{
    Mutex mutex;
    ScopedLock lock(mutex);
}


} // namespace Overloading


#endif // Overloading_H
