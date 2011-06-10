#ifndef THREADING_H_INCLUDED
#define THREADING_H_INCLUDED


#include <pthread.h>


namespace Threading {


enum ExclusionMechanism
{
    ExclusionMechanism_Normal,
    ExclusionMechanism_Recursive
};


// Traits class
template<ExclusionMechanism>
struct Traits;


template<>
struct Traits<ExclusionMechanism_Normal>
{
    static pthread_mutex_t GetNativeMutex()
    {
        pthread_mutex_t result = PTHREAD_MUTEX_INITIALIZER;
        return result;
    }
};


template<>
struct Traits<ExclusionMechanism_Recursive>
{
    pthread_mutex_t GetNativeMutex()
    {
        pthread_mutex_t result = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
        return result;
    }
};


static void Lock(pthread_mutex_t & inMutex)
{
    pthread_mutex_lock(&inMutex);
}


static void Unlock(pthread_mutex_t & inMutex)
{
    pthread_mutex_unlock(&inMutex);
}


// Mutex class
template<ExclusionMechanism>
class GenericMutex;


template<ExclusionMechanism Exclusion_>
class GenericScopedLock
{
public:
    GenericScopedLock(GenericMutex<Exclusion_> & inMutex) :
        mMutex(inMutex)
    {
        mMutex.lock();
    }

    ~GenericScopedLock()
    {
        mMutex.unlock();
    }

private:
    GenericScopedLock(const GenericScopedLock&);
    GenericScopedLock& operator=(const GenericScopedLock&);

    GenericMutex<Exclusion_> & mMutex;
};


template<ExclusionMechanism Exclusion_>
class GenericMutex
{
public:
    typedef GenericScopedLock<Exclusion_> ScopedLock;

    GenericMutex() :
        mNativeMutex(Traits<Exclusion_>::GetNativeMutex())
    {
    }

    ~GenericMutex()
    {
    }

    void lock() { Lock(mNativeMutex); }

    void unlock() { Unlock(mNativeMutex); }

private:
    GenericMutex(const GenericMutex&);
    GenericMutex& operator=(const GenericMutex&);

    friend class GenericScopedLock<Exclusion_>;

    pthread_mutex_t mNativeMutex;
};


// Define the FastMutex type
typedef GenericMutex<ExclusionMechanism_Normal> FastMutex;


// Define the RecursiveMutex type
typedef GenericMutex<ExclusionMechanism_Recursive> RecursiveMutex;


} // namespace Threading


#endif // THREADING_H_INCLUDED
