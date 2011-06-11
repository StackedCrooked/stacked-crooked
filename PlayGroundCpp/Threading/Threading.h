#ifndef THREADING_H_INCLUDED
#define THREADING_H_INCLUDED


#include <boost/noncopyable.hpp>
#include <algorithm>
#include <pthread.h>


//
// Emulate namespace with templated struct
//
template<class MutexType>
struct GenericThreading {


typedef MutexType Mutex;
template<class> class ScopedAccessor;


class ScopedLock : boost::noncopyable
{
public:
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
    Mutex & mMutex;
};




/**
 * ThreadSafe can be used to create a thread-safe object.
 * Access to the held object can be obtained by creating a ScopedAccessor object.
 */
template<class Variable>
class ThreadSafe
{
public:
    ThreadSafe() :
        mData(new Data(new Variable()))
    {
    }

    ThreadSafe(Variable * inVariable) :
        mData(new Data(inVariable))
    {
    }

    ThreadSafe(const ThreadSafe<Variable> & rhs) :
        mData(rhs.mData)
    {
        ++mData->mRefCount;
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
template<typename Variable>
class ScopedAccessor
{
public:
    typedef ScopedAccessor<Variable> This;

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
    ScopedAccessor(const This & rhs);
    ScopedAccessor& operator=(const This & rhs);

    ThreadSafe<Variable> & mThreadSafeVariable;
    ScopedLock mScopedLock;
};


}; // struct GenericThreading

#endif // THREADING_H_INCLUDED
