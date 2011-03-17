#include <cassert>
#include <iostream>
#include <vector>


namespace Threading {


class Noncopyable
{
protected:
  Noncopyable() {}
  ~Noncopyable() {}
private:  // emphasize the following members are private
  Noncopyable( const Noncopyable& );
  const Noncopyable& operator=( const Noncopyable& );
};


template<class MutexType>
void LockMutex(MutexType & inMutex)
{
    inMutex.lock();
}


template<class MutexType>
void UnlockMutex(MutexType & inMutex)
{
    inMutex.unlock();
}


template<class SubType>
class Singleton : Noncopyable
{
public:
    // Create a scoped object in your main function.
    struct Initializer : Noncopyable
    {
        Initializer()
        { Singleton<SubType>::CreateInstance(); }
        
        ~Initializer()
        { Singleton<SubType>::DestroyInstance(); }
    };

    static SubType & Instance()
    {
        assert(sInstance);
        return *sInstance;
    }

protected:
    Singleton() {}
    ~Singleton() {}

private:
    static void CreateInstance()
    {
        assert(!sInstance);
        sInstance = new SubType;
    }

    static void DestroyInstance()
    {
        delete sInstance;
        sInstance = 0;
    }
    
    static SubType * sInstance;
};


template<class SubType>
SubType * Singleton<SubType>::sInstance(0);


class AbstractMutex : Noncopyable
{
public:
    AbstractMutex(int inRank) :
        mRank(inRank)
    {
    }

    virtual ~AbstractMutex()
    {
    }

    int rank() const
    {
        return mRank;
    }

private:
    int mRank;
};


// Forward declaration
template<class RankedMutexType>
class RankedScopedLock;


template<class RankedMutexType, class MutexType>
class GenericRankedMutex : public AbstractMutex
{
public:
    typedef RankedMutexType RankedMutex;
    typedef typename MutexType NativeMutex;
    typedef RankedScopedLock<RankedMutex> ScopedLock;

    GenericRankedMutex() :
        AbstractMutex(RankedMutex::Layer)
    {
    }

    virtual ~GenericRankedMutex()
    {
    }

    NativeMutex & getMutex()
    {
        return mMutex;
    }

    const NativeMutex & getMutex() const
    {
        return mMutex;
    }

private:
    NativeMutex mMutex;
};


template<class NativeMutexType>
class RootMutex : public GenericRankedMutex<RootMutex<NativeMutexType>, NativeMutexType>
{
public:
    enum { Layer = 0 };
};


template<class LowerMutex>
class RankedMutex : public GenericRankedMutex<RankedMutex<LowerMutex>, typename LowerMutex::NativeMutex>
{
public:
    typedef LowerMutex LowerRankedMutex;

    enum { Layer = LowerMutex::Layer + 1 };
};


template<class MutexType>
class RankChecker : public Singleton<RankChecker<MutexType> >
{
public:
    typedef MutexType NativeMutex;

    template<class MutexType>
    void push(MutexType & inMutex)
    {
        LockMutex(mGlobalRanksMutex);
        if (mGlobalRanks.empty() || inMutex.rank() <= mGlobalRanks.back())
        {
            mGlobalRanks.push_back(inMutex.rank());
            UnlockMutex(mGlobalRanksMutex);
        }
        else
        {
            UnlockMutex(mGlobalRanksMutex);
            throw std::runtime_error("Layer is higher than previous.");
        }
    }

    void pop()
    {
        LockMutex(mGlobalRanksMutex);
        mGlobalRanks.pop_back();
        UnlockMutex(mGlobalRanksMutex);
    }

private:
    std::vector<int> mGlobalRanks;
    NativeMutex mGlobalRanksMutex;
};


template<class RankedMutexType>
class RankedScopedLock : Noncopyable
{
public:
    typedef RankedMutexType RankedMutex;
    typedef typename RankedMutex::NativeMutex NativeMutex;

    RankedScopedLock(RankedMutexType & inRankedMutex) :
        mRankedMutex(inRankedMutex)
    {
        RankChecker<NativeMutex>::Instance().push(mRankedMutex);
        LockMutex(mRankedMutex.getMutex());
    }

    ~RankedScopedLock()
    {
        UnlockMutex(mRankedMutex.getMutex());
        RankChecker<NativeMutex>::Instance().pop();
    }

private:
    RankedMutex & mRankedMutex;
};


template<class NativeMutexType>
class Initializer
{
public:
    typedef NativeMutexType NativeMutex;
    typedef RankChecker<NativeMutex> RankChecker;
    typedef RootMutex<NativeMutex> RootMutex;    

    Initializer() : mRankCheckerInitializer()
    {
    }
        
private:
    typedef typename RankChecker::Initializer RankCheckerInit;
    RankCheckerInit mRankCheckerInitializer;
};


} // namespace Threading


#include "Poco/Foundation.h"
#include "Poco/Mutex.h"


// Choose a native mutex type.
typedef Threading::Initializer<Poco::Mutex> ThreadInitializer;



int main()
{
    ThreadInitializer scopedThreadInitializer;

    typedef ThreadInitializer::RootMutex RootMutex;
    typedef Threading::RankedMutex<RootMutex> L1Mutex;
    typedef Threading::RankedMutex<L1Mutex> L2Mutex;

    RootMutex m0;
    L1Mutex m1;
    L2Mutex m2;
    
    try
    {
        std::cout << "Good locking: ";
        L2Mutex::ScopedLock l2(m2);
        L1Mutex::ScopedLock l1(m1);
        RootMutex::ScopedLock l0(m0);
        std::cout << "OK" << std::endl;
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
    }

    try
    {
        std::cout << "Bad locking: ";
        RootMutex::ScopedLock l0(m0);
        L2Mutex::ScopedLock l2(m2);
        L1Mutex::ScopedLock l1(m1);
        std::cout << "An exception should have been thrown. Something is wrong here." << std::endl;
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
    }
    return 0;
}

