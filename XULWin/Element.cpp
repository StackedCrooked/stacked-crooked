#include "Element.h"
#include "Window.h"
#include "WindowFactory.h"


namespace XULWin
{
		
	Element::Element(Element * inParent, const Type & inType, const ID & inID, NativeComponent * inNativeWindow) :
		mParent(inParent),
		mType(inType),
		mID(inID),
		mNativeWindow(inNativeWindow)
	{
		if (inParent)
		{
			inParent->add(this);
		}
	}


	const Element::Type & Element::type() const
	{
		return mType;
	}


	const Element::ID & Element::id() const
	{
		return mID;
	}
	
	
	NativeComponent * Element::window() const
	{
		return mNativeWindow;
	}
	
	
	void Element::add(Element * inChild)
	{
		mChildren.insert(inChild);
	}


	Window::Window(const ID & inID) :
		Element(0, Element::Type("window"), inID, new NativeWindow)
	{
	}


	Button::Button(Element * inParent, const ID & inID) :
		Element(inParent, Element::Type("button"), inID, new NativeButton(inParent->window()))
	{
	}

} // XULWin
