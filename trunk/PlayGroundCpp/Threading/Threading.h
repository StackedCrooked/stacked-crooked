#ifndef THREADING_H_INCLUDED
#define THREADING_H_INCLUDED


#include <pthread.h>
#include <algorithm>


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


template<class Variable>
class ScopedAccessor;


template<class Variable>
class ThreadSafe
{
public:
    ThreadSafe(Variable * inVariable) :
        mData(new Data(inVariable))
    {
    }

    ThreadSafe(const ThreadSafe<Variable> & rhs) :
        mData(rhs.mData)
    {
        mData->mRefCount++;
    }

    ThreadSafe<Variable> & operator=(const ThreadSafe<Variable> & rhs)
    {
        // Implement operator= using the copy/swap idiom:
        ThreadSafe<Variable> tmp(rhs);
        std::swap(mData, tmp.mData);
        return *this;
    }

    ~ThreadSafe()
    {
        if (--mData->mRefCount == 0)
        {
            delete mData;
        }
    }

private:
    friend class ScopedAccessor<Variable>;

    FastMutex & getMutex() { return mData->mMutex; }

    const Variable & getVariable() const { return *mData->mVariable; }

    Variable & getVariable() { return *mData->mVariable; }

    struct Data
    {
        Data(Variable * inVariable) :
            mVariable(inVariable),
            mMutex(),
            mRefCount(1)
        {
        }
        Variable * mVariable;
        FastMutex mMutex;
        unsigned mRefCount;
    };

    Data * mData;
};


template<typename Variable>
class ScopedAccessor
{
public:
    ScopedAccessor(ThreadSafe<Variable> & inThreadSafeVariable) :
        mThreadSafeVariable(inThreadSafeVariable),
        mScopedLock(inThreadSafeVariable.getMutex())
    {
    }

    const Variable & get() const { return mThreadSafeVariable.getVariable(); }

    Variable & get() { return mThreadSafeVariable.getVariable(); }

private:
    // non-copyable
    ScopedAccessor(const ScopedAccessor<Variable> & rhs);
    ScopedAccessor& operator=(const ScopedAccessor<Variable>& rhs);

    ThreadSafe<Variable> & mThreadSafeVariable;
    FastMutex::ScopedLock mScopedLock;
};


} // namespace Threading


#endif // THREADING_H_INCLUDED
