#ifndef OOP_H
#define OOP_H


#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <utility>


namespace OOP {


class NativeMutex;


class Mutex
{
public:
    Mutex();

    // allow move construction and move assignment
    Mutex(Mutex&&) = default;
    Mutex& operator=(Mutex&&) = default;

    void lock();

    void unlock();

private:
    // disallow copy construction and copy assignment
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    std::unique_ptr<NativeMutex> mNativeMutex;
};


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

} // namespace OOP


#include <pthread.h>


namespace OOP {


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


} // namespace OOP


#endif // OOP_H
