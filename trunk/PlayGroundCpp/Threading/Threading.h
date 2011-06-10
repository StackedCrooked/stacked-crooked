#ifndef THREADING_H_INCLUDED
#define THREADING_H_INCLUDED


#include <pthread.h>
#include <algorithm>


namespace Threading {


static pthread_mutex_t GetFastNativeMutex()
{
    pthread_mutex_t result = PTHREAD_MUTEX_INITIALIZER;
    return result;
}


pthread_mutex_t GetRecursiveNativeMutex()
{
    pthread_mutex_t result = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    return result;
}


static void Lock(pthread_mutex_t & inMutex)
{
    pthread_mutex_lock(&inMutex);
}


static void Unlock(pthread_mutex_t & inMutex)
{
    pthread_mutex_unlock(&inMutex);
}


template<class MutexType>
class ScopedLock
{
public:
    typedef MutexType Mutex;

    ScopedLock(Mutex & inMutex) :
        mMutex(inMutex)
    {
        mMutex.lock();
    }

    ~ScopedLock()
    {
        mMutex.unlock();
    }

private:
    ScopedLock(const ScopedLock&);
    ScopedLock& operator=(const ScopedLock&);

    Mutex & mMutex;
};


class FastMutex
{
public:
    typedef Threading::ScopedLock<FastMutex> ScopedLock;

    FastMutex() :
        mNativeMutex(GetFastNativeMutex())
    {
    }

    ~FastMutex()
    {
    }

    void lock() { Lock(mNativeMutex); }

    void unlock() { Unlock(mNativeMutex); }

private:
    FastMutex(const FastMutex&);
    FastMutex& operator=(const FastMutex&);

    friend class Threading::ScopedLock<FastMutex>;

    pthread_mutex_t mNativeMutex;
};


class RecursiveMutex
{
public:
    typedef Threading::ScopedLock<RecursiveMutex> ScopedLock;

    RecursiveMutex() :
        mNativeMutex(GetRecursiveNativeMutex())
    {
    }

    ~RecursiveMutex()
    {
    }

    void lock() { Lock(mNativeMutex); }

    void unlock() { Unlock(mNativeMutex); }

private:
    RecursiveMutex(const RecursiveMutex&);
    RecursiveMutex& operator=(const RecursiveMutex&);

    friend class Threading::ScopedLock<RecursiveMutex>;

    pthread_mutex_t mNativeMutex;
};


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

    const Variable * operator->() const { return &mThreadSafeVariable.getVariable(); }

    Variable * operator->() { return &mThreadSafeVariable.getVariable(); }

private:
    // non-copyable
    ScopedAccessor(const ScopedAccessor<Variable> & rhs);
    ScopedAccessor& operator=(const ScopedAccessor<Variable>& rhs);

    ThreadSafe<Variable> & mThreadSafeVariable;
    FastMutex::ScopedLock mScopedLock;
};


} // namespace Threading


#endif // THREADING_H_INCLUDED
