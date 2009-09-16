#include "Element.h"
#include "Window.h"


namespace XULWin
{


    Element::Element(ElementPtr inParent, const Type & inType, const ID & inID, boost::shared_ptr<NativeComponent> inNativeComponent) :
        mParent(inParent),
        mType(inType),
        mID(inID),
        mNativeComponent(inNativeComponent)
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
    
    
    boost::shared_ptr<NativeComponent> Element::nativeComponent() const
    {
        return mNativeComponent;
    }
    
    
    void Element::add(ElementPtr inChild)
    {
        mChildren.insert(inChild);
    }


    Window::Window(const ID & inID) :
        Element(ElementPtr(),
                Element::Type("window"),
                inID,
                NativeComponentPtr(new NativeWindow))
    {
    }


    Button::Button(ElementPtr inParent, const ID & inID) :
        Element(inParent,
                Element::Type("button"),
                inID,
                NativeComponentPtr(new NativeButton(inParent->nativeComponent())))
    {
    }

} // XULWin
