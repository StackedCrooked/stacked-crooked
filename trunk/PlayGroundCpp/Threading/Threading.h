#ifndef THREADING_H_INCLUDED
#define THREADING_H_INCLUDED


#include <boost/noncopyable.hpp>
#include <algorithm>
#include <pthread.h>



namespace Threading {


template<class, class> class ScopedAccessor;


template<class Mutex>
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
template<class Variable, class Mutex>
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
    typedef ScopedAccessor<Variable, Mutex> This;

    ScopedAccessor(ThreadSafe<Variable, Mutex> & inThreadSafeVariable) :
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

    ThreadSafe<Variable, Mutex> & mThreadSafeVariable;
    ScopedLock<Mutex> mScopedLock;
};


namespace Posix {


struct Mutex : boost::noncopyable
{
    Mutex() { pthread_mutex_init(&mMutex, NULL); }

    ~Mutex() { pthread_mutex_destroy(&mMutex); }

    pthread_mutex_t & getMutex() { return mMutex; }

    const pthread_mutex_t & getMutex() const { return mMutex; }

    void lock() { pthread_mutex_lock(&mMutex); }

    void unlock() { pthread_mutex_unlock(&mMutex); }

    pthread_mutex_t mMutex;
};



typedef Threading::ScopedLock<Mutex> ScopedLock;


template<class> class ThreadSafe;


//
// We use inheritance to emulate template typedefs which
// are not supported in C++03 (but will be in C++11).
//
template<class Variable>
class ScopedAccessor : public Threading::ScopedAccessor<Variable, Mutex>
{
public:
    typedef Threading::ScopedAccessor<Variable, Mutex> Base;

    ScopedAccessor(ThreadSafe<Variable> & inThreadSafe) : Base(inThreadSafe) { }
};


template<class Variable>
class ThreadSafe : public Threading::ThreadSafe<Variable, Mutex>
{
public:
    typedef Threading::ThreadSafe<Variable, Mutex> Base;

    ThreadSafe() : Base() { }

    ThreadSafe(Variable * inVariable) : Base(inVariable) { }

    ThreadSafe(const Threading::ThreadSafe<Variable, Mutex> & rhs) : Base(rhs) {}
};


} // namespace Posix
} // namespace Threading


#endif // THREADING_H_INCLUDED
