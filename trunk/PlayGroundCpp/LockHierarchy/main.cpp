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


template<class NativeMutexType>
struct GenericThreadLibrary
{
    typedef NativeMutexType NativeMutex;
    
    static void LockMutex(NativeMutex & inNativeMutex)
    {
        inNativeMutex.lock();
    }
    
    static void UnlockMutex(NativeMutex & inNativeMutex)
    {
        inNativeMutex.unlock();
    }


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
    template<class HMutexType>
    class ScopedLock;


    template<class ThisType>
    class GenericRankedMutex : public AbstractMutex
    {
    public:
        typedef ThisType This;
        typedef ScopedLock<This> ScopedLock;

        GenericRankedMutex() :
            AbstractMutex(This::Level)
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

    
    class Level0Mutex : public GenericRankedMutex<Level0Mutex>
    {
    public:
        enum { Level = 0 };
    };


    template<class LowerMutexType>
    class LevelNMutex : public GenericRankedMutex< LevelNMutex<LowerMutexType> >
    {
    public:
        typedef LowerMutexType LowerMutex;

        enum { Level = LowerMutex::Level + 1 };
    };


    typedef LevelNMutex<Level0Mutex> Level1Mutex;
    typedef LevelNMutex<Level1Mutex> Level2Mutex;
    typedef LevelNMutex<Level2Mutex> Level3Mutex;
    typedef LevelNMutex<Level3Mutex> Level4Mutex;
    typedef LevelNMutex<Level4Mutex> Level5Mutex;
    typedef LevelNMutex<Level5Mutex> Level6Mutex;
    typedef LevelNMutex<Level6Mutex> Level7Mutex;
    typedef LevelNMutex<Level7Mutex> Level8Mutex;
    typedef LevelNMutex<Level8Mutex> Level9Mutex;


    class RankChecker : public Singleton<RankChecker>
    {
    public:
        void push(int inRank)
        {
            LockMutex(mNativeMutex);
            if (mCurrentRanks.empty() || inRank < mCurrentRanks.back())
            {
                mCurrentRanks.push_back(inRank);
                UnlockMutex(mNativeMutex);
            }
            else
            {
                UnlockMutex(mNativeMutex);
                throw std::runtime_error("Level is higher than previous.");
            }
        }

        void pop()
        {
            LockMutex(mNativeMutex);
            mCurrentRanks.pop_back();
            UnlockMutex(mNativeMutex);
        }

    private:
        std::vector<int> mCurrentRanks;
        NativeMutex mNativeMutex;
    };


    template<class HierarchicalMutexType>
    class ScopedLock : Noncopyable
    {
    public:
        typedef HierarchicalMutexType LevelNMutex;

        ScopedLock(LevelNMutex & inHierarchicalMutex) :
            mHierarchicalMutex(inHierarchicalMutex)
        {
            RankChecker::Instance().push(mHierarchicalMutex.rank());
            LockMutex(mHierarchicalMutex.getMutex());
        }

        ~ScopedLock()
        {
            UnlockMutex(mHierarchicalMutex.getMutex());
            RankChecker::Instance().pop();
        }

    private:
        LevelNMutex & mHierarchicalMutex;
    };


    // Create a scoped and named object to initalize the treading lib;
    struct Initializer
    {
    private:
        typename Singleton<RankChecker>::Initializer mRankCheckerInitializer;


#define INIT_THREAD_LIBRARY(NativeMutexType) \
    typedef GenericThreadLibrary<NativeMutexType> ThreadLibrary; \
    typedef ThreadLibrary::Level0Mutex Level0Mutex; \
    typedef ThreadLibrary::Level1Mutex Level1Mutex; \
    typedef ThreadLibrary::Level2Mutex Level2Mutex;
    };


}; // struct GenericThreadLibrary



//
// BEGIN USER CODE
//
#include "Poco/Foundation.h"
#include "Poco/Mutex.h"


//
// Init the thread library using Poco::Mutex.
//
INIT_THREAD_LIBRARY(Poco::Mutex)


//
// Main function
//
int main()
{
    ThreadLibrary::Initializer theThreadLibraryInitializer;

    Level0Mutex theLevel0Mutex;
    Level1Mutex theLevel1Mutex;
    Level2Mutex theLevel2Mutex;
    
    try
    {
        Level2Mutex::ScopedLock level2Lock(theLevel2Mutex);
        Level1Mutex::ScopedLock level1Lock(theLevel1Mutex);
        Level0Mutex::ScopedLock level0Lock(theLevel0Mutex);
        std::cout << "PASS" << std::endl;
    }
    catch (const std::exception & exc)
    {
        std::cout << "FAIL: " << exc.what() << std::endl;
    }

    try
    {
        Level1Mutex::ScopedLock level1Lock(theLevel1Mutex);
        Level0Mutex::ScopedLock level0Lock(theLevel0Mutex);
        Level2Mutex::ScopedLock level2Lock(theLevel2Mutex);
        std::cout << "FAIL" << std::endl;
    }
    catch (const std::exception & )
    {
        std::cout << "PASS" << std::endl;
    }
    return 0;
}

