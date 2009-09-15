#include "ElementFactory.h"
#include "Element.h"
#include <assert.h>


namespace XULWin
{

	void ElementFactory::Initialize()
	{
		assert (!sInstance);
		sInstance = new ElementFactory;
	}


	ElementFactory & ElementFactory::Instance()
	{
		return *sInstance;
	}


	void ElementFactory::Finalize()
	{
		assert(sInstance);
		delete sInstance;
	}

	
	Element * ElementFactory::createElement(const std::string & inType, const std::string & inID)
	{
	}

	
	HWND ElementFactory::getWindowHandle(const std::string & inID) const
	{
	}


} // namespace XULWin
