#include "WindowFactory.h"
#include <assert.h>


namespace XULWin
{

	WindowFactory * WindowFactory::sInstance = 0;

	
	void WindowFactory::Initialize()
	{
		assert (!sInstance);
		sInstance = new WindowFactory;
	}


	WindowFactory & WindowFactory::Instance()
	{
		return *sInstance;
	}


	void WindowFactory::Finalize()
	{
		assert(sInstance);
		delete sInstance;
	}

	
	HWND WindowFactory::create(const std::string & inType)
	{
		//::CreateWindowEx(
		return 0;
	}

	
	HWND WindowFactory::get(const std::string & inID) const
	{
		return 0;
	}


} // namespace XULWin
