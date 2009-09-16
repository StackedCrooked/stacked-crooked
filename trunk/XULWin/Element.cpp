#include "Element.h"
#include "NativeComponent.h"
#include "ErrorHandler/ErrorStack.h"


namespace XULWin
{


    Element::Element(const Type & inType, ElementPtr inParent, boost::shared_ptr<NativeComponent> inNativeComponent) :
        mType(inType),
        mParent(inParent),
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


    Window::Window(const Type & inType) :
        Element(inType,
                ElementPtr(),
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


    Button::Button(const Type & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeButton(inParent->nativeComponent())))
    {
    }


    Label::Label(const Type & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeLabel(inParent->nativeComponent())))
    {
    }


    CheckBox::CheckBox(const Type & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeCheckBox(inParent->nativeComponent())))
    {
    }


    HBox::HBox(const Type & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeHBox(inParent->nativeComponent())))
    {
    }


    VBox::VBox(const Type & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeVBox(inParent->nativeComponent())))
    {
    }


} // XULWin
