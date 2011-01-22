#include "MyClass.h"
#include <iostream>


#define TRACE std::cout << __FUNCTION__ << std::endl;


struct MyClass::Impl
{
	Impl(const std::string & inName) :
		mName(inName)
	{
		TRACE
	}

	~Impl()
	{
		TRACE
	}
	std::string mName;
};


MyClass::MyClass(const std::string & inName) :
	mImpl(new Impl(inName))
{
	TRACE
}


MyClass::~MyClass()
{
	TRACE
}


const std::string & MyClass::name() const
{
	return mImpl->mName;
}

