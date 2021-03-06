#ifndef POLICY_H
#define POLICY_H


#include <pthread.h>


namespace Policy {


enum class Recursive { Yes, No };
enum class Spin      { Yes, No };


namespace Posix {


template<Recursive>
struct Attribute
{
    pthread_mutexattr_t * getAttribute()
    {
        return nullptr;
    }
};


template<>
struct Attribute<Recursive::Yes>
{
    Attribute()
    {
        pthread_mutexattr_init(&obj);
        pthread_mutexattr_settype(&obj, PTHREAD_MUTEX_RECURSIVE);
    }

    ~Attribute()
    {
        pthread_mutexattr_destroy(&obj);
    }

    pthread_mutexattr_t * getAttribute()
    {
        return &obj;
    }

    pthread_mutexattr_t obj;
};


template<Recursive, Spin>
struct NativeMutex;


template<Recursive rec>
struct NativeMutex<rec, Spin::No> : Attribute<rec>
{
    typedef pthread_mutex_t Type;

    NativeMutex()
    {
        pthread_mutex_init(&obj, this->getAttribute());
    }

    ~NativeMutex()
    {
        pthread_mutex_destroy(&obj);
    }

    void lock()    { pthread_mutex_lock (&obj); }
    void unlock()  { pthread_mutex_unlock(&obj); }

    Type obj;
};


#ifdef __APPLE__
typedef int pthread_spinlock_t;
template<typename ...Args> void pthread_spin_init(Args && ...)    {}
template<typename ...Args> void pthread_spin_destroy(Args && ...) {}
template<typename ...Args> void pthread_spin_lock(Args && ...)    {}
template<typename ...Args> void pthread_spin_unlock(Args && ...)  {}
#endif // __APPLE__



template<>
struct NativeMutex<Recursive::No, Spin::Yes>
{
    typedef pthread_spinlock_t Type;

    NativeMutex()
    {
        pthread_spin_init(&obj, PTHREAD_PROCESS_PRIVATE);
    }

    ~NativeMutex()
    {
        pthread_spin_destroy(&obj);
    }

    void lock()    { pthread_spin_lock  (&obj); }
    void unlock()  { pthread_spin_unlock(&obj); }

    Type obj;
};


} // namespace Posix



#ifdef __APPLE__
using namespace Posix;
#else
using namespace Posix;
#endif


typedef NativeMutex< Recursive::No , Spin::No  > Mutex;
typedef NativeMutex< Recursive::Yes, Spin::No  > RecursiveMutex;
typedef NativeMutex< Recursive::No , Spin::Yes > SpinMutex;


} // namespace Policy


#endif // POLICY_H
