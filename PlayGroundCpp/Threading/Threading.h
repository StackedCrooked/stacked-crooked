#ifndef THREADING_H_INCLUDED
#define THREADING_H_INCLUDED


namespace Threading {


enum LockType
{
    LockType_Fast,
    LockType_Recursive
};


// Mutex class
template<LockType LockType_> class Mutex;

// ScopedLock class
template<typename> class ScopedLock;

// Traits class
template<LockType> struct Traits;


template<>
struct Traits<LockType_Fast>
{
    typedef Mutex<LockType_Fast> Mutex;

    static pthread_mutex_t GetNativeMutex()
    {
        pthread_mutex_t result = PTHREAD_MUTEX_INITIALIZER_NP;
        return result;
    }

    static void Lock(Mutex & inMutex)
    { pthread_mutex_lock(inMutex.GetNativeMutex()); }

    static void Unlock(Mutex & inMutex)
    { pthread_mutex_unlock(inMutex.GetNativeMutex()); }
};


template<>
struct Traits<LockType_Recursive>
{
    typedef Mutex<LockType_Recursive> Mutex;

    pthread_mutex_t GetNativeMutex()
    {
        pthread_mutex_t result = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
        return result;
    }

    static void Lock(Mutex & inMutex)
    { pthread_mutex_lock(inMutex.GetNativeMutex()); }

    static void Unlock(Mutex & inMutex)
    { pthread_mutex_unlock(inMutex.GetNativeMutex()); }
};


template<LockType LockType_>
class Mutex
{
public:
    typedef Mutex<LockType_> This;
    typedef Traits<LockType_> Traits;
    typedef ScopedLock<LockType_> ScopedLock;

    Mutex() :
        mMutex(Traits::GetNativeMutex())
    {
    }

    ~Mutex()
    {
    }

    void lock() { Lock(mMutex); }

    void unlock() { Unlock(mMutex); }

private:
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);

    friend class ScopedLock<LockType_>;

    pthread_mutex_t mMutex;
};


template<LockType LockType_>
class ScopedLock
{
public:
    typedef Mutex<LockType_> Mutex;

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


// Define the FastMutex type
typedef Mutex<LockType_Fast> FastMutex;


// Define the RecursiveMutex type
typedef Mutex<LockType_Recursive> RecursiveMutex;


} // namespace Threading


#endif // THREADING_H_INCLUDED

