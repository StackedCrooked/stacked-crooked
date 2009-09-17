#include "Element.h"
#include "NativeComponent.h"
#include "ErrorHandler/ErrorStack.h"
#include <boost/bind.hpp>


namespace XULWin
{
    NativeComponentPtr gNullNativeComponent;


    Element::Element(const Type & inType, ElementPtr inParent, boost::shared_ptr<NativeComponent> inNativeComponent) :
        mType(inType),
        mParent(inParent),
        mNativeComponent(inNativeComponent)
    {
        if (mNativeComponent)
        {
            mNativeComponent->setOwningElement(this);
        }
    }

    
    void Element::onStart()
    {
        if (mNativeComponent)
        {
            mNativeComponent->onStart();
        }
    }

    
    void Element::onEnd()
    {
        if (mNativeComponent)
        {
            mNativeComponent->onEnd();
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
    
    
    boost::shared_ptr<NativeComponent> Element::nativeComponent() const
    {
        return mNativeComponent;
    }
    
    
    void Element::addChild(ElementPtr inChild)
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
        static_cast<NativeWindow *>(nativeComponent().get())->showModal();
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


    Box::Box(const Type & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeBox(inParent->nativeComponent())))
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


    MenuList::MenuList(const Type & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeMenuList(inParent->nativeComponent())))
    {
    }


    void MenuList::onEnd()
    {
        if (!mChildren.empty())
        {
            // Find an entry with name 'menupopup'
            Children::iterator popupIt = std::find_if
            (
                mChildren.begin(),
                mChildren.end(),
                boost::bind
                (
                    &Element::type,
                    boost::bind(&ElementPtr::get, _1)
                ) == eltype("menupopup")
            );

            if (popupIt == mChildren.end())
            {
                return;
            }

            MenuPopup * popup = static_cast<MenuPopup*>(popupIt->get());
            
            // Find an entry with name 'menulist'
            Children::iterator itemIt = popup->children().begin(), end = popup->children().end();
            for (; itemIt != end; ++itemIt)
            {
                if (itemIt->get()->type() == eltype("menuitem"))
                {
                    MenuItem * item = static_cast<MenuItem *>(itemIt->get());
                    NativeMenuList * menuList = static_cast<NativeMenuList *>(nativeComponent().get());
                    menuList->add(item->Attributes["label"]);
                }
            }
        }
    }

    
    MenuPopup::MenuPopup(const Type & inType, ElementPtr inParent) :
        Element(inType, inParent, gNullNativeComponent)
    {
    }

    
    MenuItem::MenuItem(const Type & inType, ElementPtr inParent) :
        Element(inType, inParent, gNullNativeComponent)
    {
    }


} // XULWin
