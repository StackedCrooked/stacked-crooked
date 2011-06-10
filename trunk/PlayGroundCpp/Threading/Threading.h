#ifndef THREADING_H_INCLUDED
#define THREADING_H_INCLUDED


#include <boost/noncopyable.hpp>
#include <algorithm>
#include <pthread.h>


namespace Threading {


template<class MutexType>
struct ScopedLock : boost::noncopyable
{
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

    Mutex & mMutex;
};


struct PosixMutex : boost::noncopyable
{
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


template<class VariableType,
         class MutexType>
class ThreadSafe
{
public:
    typedef VariableType Variable;
    typedef MutexType Mutex;
    typedef ThreadSafe<Variable, Mutex> This;

    ThreadSafe(Variable * inVariable) :
        mData(new Data(inVariable))
    {
    }

    ThreadSafe(const This & rhs) :
        mData(rhs.mData)
    {
        ++mData->mRefCount;
    }

    This & operator=(const This & rhs)
    {
        // Implement operator= using the copy/swap idiom:
        This tmp(rhs);
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


template<typename VariableType,
         class MutexType>
class ScopedAccessor
{
public:
    typedef VariableType Variable;
    typedef MutexType Mutex;
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
