#include "WindowFactory.h"
#include "Window.h"


namespace XULWin
{

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

	
	HWND WindowFactory::create(const std::string & inType) const
	{
	}

	
	HWND WindowFactory::get(const std::string & inID) const
	{
	}


} // namespace XULWin
