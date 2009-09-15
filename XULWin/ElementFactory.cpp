#include "ElementFactory.h"
#include "Element.h"
#include <assert.h>


namespace XULWin
{

	ElementFactory * ElementFactory::sInstance = 0;


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

	
	Element * ElementFactory::create(const std::string & inType, const std::string & inID)
	{
		return 0;
	}

	
	Element * ElementFactory::get(const std::string & inID) const
	{
		return 0;
	}


} // namespace XULWin
