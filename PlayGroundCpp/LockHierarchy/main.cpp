#include "Poco/Foundation.h"
#include "Poco/Mutex.h"
#include <iostream>
#include <vector>


typedef Poco::Mutex Mutex;
void LockMutex(Mutex & inMutex)
{
	inMutex.lock();
}


void UnlockMutex(Mutex & inMutex)
{
	inMutex.unlock();
}


static std::vector<int> sGlobalRanks;
static Mutex sGlobalRanksMutex;


template<class _RankedMutexType>
class RankedMutexLock
{
public:
	typedef _RankedMutexType RankedMutex;
	RankedMutexLock(_RankedMutexType & inRankedMutex) :
		mRankedMutex(inRankedMutex)
	{
        CheckRank();
		LockMutex(mRankedMutex.getMutex());
	}

	~RankedMutexLock()
	{
		UnlockMutex(mRankedMutex.getMutex());

        LockMutex(sGlobalRanksMutex);
        sGlobalRanks.pop_back();
        UnlockMutex(sGlobalRanksMutex);
	}

private:
	RankedMutexLock(const RankedMutexLock &);
	RankedMutexLock& operator=(const RankedMutexLock &);

    void CheckRank()
    {
        LockMutex(sGlobalRanksMutex);
        //std::cout << "sGlobalRanks.size(): " << sGlobalRanks.size() << std::endl;
        if (sGlobalRanks.empty() || mRankedMutex.rank() <= sGlobalRanks.back())
        {
            //std::cout << "Pushing " << mRankedMutex.rank() << std::endl;
            sGlobalRanks.push_back(mRankedMutex.rank());
            UnlockMutex(sGlobalRanksMutex);
        }
        else
        {
            UnlockMutex(sGlobalRanksMutex);
            //std::cout << "mRankedMutex.rank(): " << mRankedMutex.rank() << std::endl;
            //std::cout << "GlobalRank: " << sGlobalRanks.back()  << std::endl << std::flush;
            throw std::runtime_error("Rank is higher than previous.");
        }
    }

	RankedMutex & mRankedMutex;
};



class AbstractMutexWrapper
{
public:
    AbstractMutexWrapper(int inRank) :
        mRank(inRank)
    {
    }

    virtual ~AbstractMutexWrapper()
    {
    }

    int rank() const
    {
        return mRank;
    }

private:
    int mRank;
};



template<class _MutexType>
class MutexWrapper : public AbstractMutexWrapper
{
public:
	typedef _MutexType MutexType;

    MutexWrapper(int inRank) :
        AbstractMutexWrapper(inRank)
    {
    }

	virtual ~MutexWrapper()
	{

	}

	MutexType & getMutex()
	{
		return mMutex;
	}

	const MutexType & getMutex() const
	{
		return mMutex;
	}

protected:
private:
	MutexType mMutex;
    int mRank;
};


template<class _MutexType>
class BottomMutex : public MutexWrapper<_MutexType>
{
public:
    typedef MutexWrapper<_MutexType> Super;
	typedef BottomMutex This;
    typedef typename Super::MutexType MutexType;
	typedef RankedMutexLock<This> ScopedLock;

	enum
    {
		Rank = 0
	};


    BottomMutex() :
        Super(Rank)
    {
    }
};


template<class _LowerRankedMutex>
class RankedMutex : public MutexWrapper<typename _LowerRankedMutex::MutexType>
{
public:
	typedef MutexWrapper<typename _LowerRankedMutex::MutexType> Super;
	typedef RankedMutex<_LowerRankedMutex> This;
	typedef RankedMutexLock<This> ScopedLock;

	enum
	{
		Rank = _LowerRankedMutex::Rank + 1
	};

    RankedMutex() :
        Super(Rank)
    {
    }
	
	virtual ~RankedMutex()
	{
	}
};





typedef BottomMutex<Mutex  > L0Mutex;
typedef RankedMutex<L0Mutex>      L1Mutex;
typedef RankedMutex<L1Mutex>      L2Mutex;
typedef RankedMutex<L2Mutex>      L3Mutex;
typedef RankedMutex<L3Mutex>      L4Mutex;


int main()
{
	L0Mutex m0;
	L1Mutex m1;
	L2Mutex m2;

	try
	{
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
		std::cout << "Expect exception: "; 
		L1Mutex::ScopedLock l1(m1);
		L0Mutex::ScopedLock l0(m0);
		L2Mutex::ScopedLock l2(m2);
		std::cout << "Huh?" << std::endl;
	}
	catch (const std::exception & exc)
	{
		std::cout << "Indeed: " << exc.what() << std::endl;
	}


	return 0;
}

