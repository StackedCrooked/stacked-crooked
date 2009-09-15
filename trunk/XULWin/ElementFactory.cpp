#include "ElementFactory.h"
#include "Element.h"
#include "WindowFactory.h"
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

	
	Element * ElementFactory::create(Element * inParent, const Element::Type & inType, const Element::ID & inID)
	{
		WindowFactory::Instance().create(WindowFactory::Instance().get(inParent->id()), inType, inID);
		return 0;
	}

	
	Element * ElementFactory::get(const Element::ID & inID) const
	{
		return 0;
	}


} // namespace XULWin
