#include <cassert>
#include <iostream>
#include <vector>


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


template<class MutexType>
class RankChecker : public Singleton<RankChecker<MutexType> >
{
public:
    typedef MutexType Mutex;

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
            throw std::runtime_error("Rank is higher than previous.");
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
    Mutex mGlobalRanksMutex;
};


template<class RankedMutexType>
class RankedMutexLock : Noncopyable
{
public:
    typedef RankedMutexType RankedMutex;
    typedef typename RankedMutex::Mutex Mutex;

    RankedMutexLock(RankedMutexType & inRankedMutex) :
        mRankedMutex(inRankedMutex)
    {
        RankChecker<Mutex>::Instance().push(mRankedMutex);
        LockMutex(mRankedMutex.getMutex());
    }

    ~RankedMutexLock()
    {
        UnlockMutex(mRankedMutex.getMutex());
        RankChecker<Mutex>::Instance().pop();
    }

private:
    RankedMutex & mRankedMutex;
};


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


template<class RankedMutexType, class MutexType>
class GenericRankedMutex : public AbstractMutex
{
public:
    typedef RankedMutexType RankedMutex;
    typedef typename MutexType Mutex;
    typedef RankedMutexLock<RankedMutex> ScopedLock;

    GenericRankedMutex() :
        AbstractMutex(RankedMutex::Rank)
    {
    }

    virtual ~GenericRankedMutex()
    {
    }

    Mutex & getMutex()
    {
        return mMutex;
    }

    const Mutex & getMutex() const
    {
        return mMutex;
    }

private:
    Mutex mMutex;
};


template<class MutexType>
class BottomMutex : public GenericRankedMutex<BottomMutex<MutexType>, MutexType>
{
public:
    enum { Rank = 0 };
};


template<class LowerRankedMutexType>
class RankedMutex : public GenericRankedMutex<RankedMutex<LowerRankedMutexType>, typename LowerRankedMutexType::Mutex>
{
public:
    typedef LowerRankedMutexType LowerRankedMutex;

    enum { Rank = LowerRankedMutexType::Rank + 1 };
};


#include "Poco/Foundation.h"
#include "Poco/Mutex.h"


typedef BottomMutex<Poco::Mutex> L0Mutex;
typedef RankedMutex<L0Mutex> L1Mutex;
typedef RankedMutex<L1Mutex> L2Mutex;
typedef RankedMutex<L2Mutex> L3Mutex;
typedef RankedMutex<L3Mutex> L4Mutex;


int main()
{
    try
    {
        RankChecker<Poco::Mutex>::Initializer theRankCheckerInit;
        BottomMutex<Poco::Mutex> m;
        L0Mutex m0;
        L1Mutex m1;
        L2Mutex m2;
        std::cout << "Expect success: ";
        L2Mutex::ScopedLock l2(m2);
        L1Mutex::ScopedLock l1(m1);
        L0Mutex::ScopedLock l0(m0);
        std::cout << "OK" << std::endl;
    }
    catch (const std::exception & exc)
    {
        std::cout << exc.what() << std::endl;
    }


    try
    {
        RankChecker<Poco::Mutex>::Initializer theRankCheckerInit;
        BottomMutex<Poco::Mutex> m;
        L0Mutex m0;
        L1Mutex m1;
        L2Mutex m2;
        std::cout << "Expect exception: ";
        L0Mutex::ScopedLock l0(m0);
        L2Mutex::ScopedLock l2(m2);
        L1Mutex::ScopedLock l1(m1);
        std::cout << "Huh?" << std::endl;
    }
    catch (const std::exception & exc)
    {
        std::cout << "Indeed: " << exc.what() << std::endl;
    }
    return 0;
}

