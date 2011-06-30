#include <boost/thread.hpp>
#include <iostream>


template <typename T>
class LockingPtr {
public:
   // Constructors/destructors
   LockingPtr(volatile T& obj, boost::mutex& mtx)
       : pObj_(const_cast<T*>(&obj)),
        pMtx_(&mtx)
   {    mtx.lock();    }
   ~LockingPtr()
   {    pMtx_->unlock();    }
   // Pointer behavior
   T& operator*()
   {    return *pObj_;    }
   T* operator->()
   {   return pObj_;   }
private:
   T* pObj_;
   boost::mutex* pMtx_;
   LockingPtr(const LockingPtr&);
   LockingPtr& operator=(const LockingPtr&);
};


class Server
{
public:
    void update()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	void update() volatile
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		LockingPtr<Server> ptr(*this, const_cast<boost::mutex&>(mMutex));
		ptr->update();
	}

private:
	mutable boost::mutex mMutex;
};


int main()
{
	volatile Server s;
	s.update();
	return 0;
}
