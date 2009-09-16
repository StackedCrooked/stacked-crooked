#include "Element.h"
#include "Window.h"
#include "ErrorHandler/ErrorStack.h"


namespace XULWin
{


    Element::Element(ElementPtr inParent, const Type & inType, const ID & inID, boost::shared_ptr<NativeComponent> inNativeComponent) :
        mParent(inParent),
        mType(inType),
        mID(inID),
        mNativeComponent(inNativeComponent)
    {
        mNativeComponent->setOwningElement(this);

        // Set default attributes
        Attributes["flex"] = "1";
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
        mChildren.push_back(inChild);
    }


    Window::Window(const ID & inID) :
        Element(ElementPtr(),
                Element::Type("window"),
                inID,
                NativeComponentPtr(new NativeWindow))
    {
    }


    void Window::showModal()
    {
        ::ShowWindow(nativeComponent()->handle(), SW_SHOW);

        MSG message;
        while (GetMessage(&message, NULL, 0, 0))
        {
            HWND hActive = GetActiveWindow();
            if (! IsDialogMessage(hActive, &message))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
    }


    Button::Button(ElementPtr inParent, const ID & inID) :
        Element(inParent,
                Element::Type("button"),
                inID,
                NativeComponentPtr(new NativeButton(inParent->nativeComponent())))
    {
    }


    CheckBox::CheckBox(ElementPtr inParent, const ID & inID) :
        Element(inParent,
                Element::Type("checkbox"),
                inID,
                NativeComponentPtr(new NativeCheckBox(inParent->nativeComponent())))
    {
    }


    HBox::HBox(ElementPtr inParent, const ID & inID) :
        Element(inParent,
                Element::Type("hbox"),
                inID,
                NativeComponentPtr(new NativeHBox(inParent->nativeComponent())))
    {
    }


} // XULWin
