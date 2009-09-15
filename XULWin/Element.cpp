#include "Element.h"


namespace XULWin
{

	Element::Element(const std::string & inType, const std::string & inId) :
		mParent(0),
		mType(inType),
		mId(inId)
	{
	}
	
		
	Element::Element(Element * inParent, const std::string & inType, const std::string & inId) :
		mParent(inParent),
		mType(inType),
		mId(inId)
	{
	}


	const std::string & Element::type() const
	{
		return mType;
	}


	const std::string & Element::id() const
	{
		return mId;
	}

} // XULWin
