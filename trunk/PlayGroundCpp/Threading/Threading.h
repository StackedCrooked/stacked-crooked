#ifndef THREADING_H_INCLUDED
#define THREADING_H_INCLUDED


#include <boost/noncopyable.hpp>
#include <algorithm>
#include <pthread.h>


namespace Threading {


template<class MutexType>
struct ScopedLock : boost::noncopyable
{
    ScopedLock(MutexType & inMutex) :
        mMutex(inMutex)
    {
        mMutex.lock();
    }

    ~ScopedLock()
    {
        mMutex.unlock();
    }

    MutexType & mMutex;
};


struct PosixMutex : boost::noncopyable
{
    typedef ScopedLock<PosixMutex> ScopedLock;

    PosixMutex() { pthread_mutex_init(&mMutex, NULL); }

    ~PosixMutex() { pthread_mutex_destroy(&mMutex); }

    pthread_mutex_t & getMutex() { return mMutex; }

    const pthread_mutex_t & getMutex() const { return mMutex; }

    void lock() { pthread_mutex_lock(&mMutex); }

    void unlock() { pthread_mutex_unlock(&mMutex); }

    pthread_mutex_t mMutex;
};


template<class Variable, class Mutex>
class ScopedAccessor;


/**
 * ThreadSafe can be used to create a thread-safe object.
 * Access to the held object can be obtained by creating a ScopedAccessor object.
 */
template<class VariableType, class MutexType>
class ThreadSafe
{
public:
    typedef VariableType Variable;
    typedef MutexType Mutex;

    ThreadSafe() :
        mData(new Data(new Variable()))
    {
    }

    ThreadSafe(Variable * inVariable) :
        mData(new Data(inVariable))
    {
    }

    ThreadSafe(const ThreadSafe<Variable, Mutex> & rhs) :
        mData(rhs.mData)
    {
        ++mData->mRefCount;
    }

    ThreadSafe<Variable, Mutex> & operator=(const ThreadSafe<Variable, Mutex> & rhs)
    {
        // Implement operator= using the copy/swap idiom:
        ThreadSafe<Variable, Mutex> tmp(rhs);
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
    friend class ScopedAccessor<Variable, Mutex>;

    Mutex & getMutex() { return mData->mMutex; }

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
        Mutex mMutex;
        unsigned mRefCount;
    };

    Data * mData;
};


/**
 * ScopedAccessor creates an atomic scope that allows access
 * to the variable held by the ThreadSafe wrapper.
 */
template<typename Variable, class Mutex>
class ScopedAccessor
{
public:
    typedef ThreadSafe<Variable, Mutex> ThreadSafeVariable;
    typedef ScopedAccessor<Variable, Mutex> This;

    ScopedAccessor(ThreadSafeVariable & inThreadSafeVariable) :
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
    ScopedAccessor(const This & rhs);
    ScopedAccessor& operator=(const This & rhs);

    ThreadSafeVariable & mThreadSafeVariable;
    ScopedLock<Mutex> mScopedLock;
};


} // namespace Threading


#endif // THREADING_H_INCLUDED
