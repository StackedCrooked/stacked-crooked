#include "Element.h"


namespace XULWin
{

	Element::Element(const Type & inType, const ID & inID) :
		mParent(0),
		mType(inType),
		mID(inID)
	{
	}
	
		
	Element::Element(Element * inParent, const Type & inType, const ID & inID) :
		mParent(inParent),
		mType(inType),
		mID(inID)
	{
	}


	const Element::Type & Element::type() const
	{
		return mType;
	}


	const Element::ID & Element::id() const
	{
		return mID;
	}

} // XULWin
