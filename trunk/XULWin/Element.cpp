#include "Element.h"
#include "NativeComponent.h"
#include "Utils/ErrorReporter.h"
#include <boost/bind.hpp>


using namespace Utils;


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


    const Element::Type & Element::type() const
    {
        return mType;
    }


    const Element::ID & Element::id() const
    {
        return mID;
    }


    void Element::removeChild(const Element * inChild)
    {
        Children::iterator it = std::find_if(mChildren.begin(), mChildren.end(), boost::bind(&ElementPtr::get, _1) == inChild);
        if (it != mChildren.end())
        {
            mChildren.erase(it);
            mNativeComponent->rebuildLayout();
        }
        else
        {
            ReportError("Remove child failed because it wasn't found.");
        }
    }


    void Element::setAttributes(const AttributesMapping & inAttributes)
    {
        mAttributes = inAttributes;
        
        if (mNativeComponent)
        {
            AttributesMapping::iterator it = mAttributes.begin(), end = mAttributes.end();
            for (; it != end; ++it)
            {
                mNativeComponent->applyAttribute(it->first, it->second);
            }
        }
    }

    
    const std::string & Element::getAttribute(const std::string & inName) const
    {
        AttributesMapping::const_iterator it = mAttributes.find(inName);
        if (it!= mAttributes.end())
        {
            return it->second;
        }

        ReportError("Attribute '" + inName + "' was not found in Element");
        static std::string fNotFound;
        return fNotFound;
    }

    
    void Element::setAttribute(const std::string & inName, const std::string & inValue)
    {
        mAttributes[inName] = inValue;
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
        
    
    void MenuList::addMenuItem(const MenuItem * inItem)
    {
        NativeMenuList * menuList = static_cast<NativeMenuList *>(nativeComponent().get());
        menuList->addMenuItem(inItem->getAttribute("label"));
    }
        
    
    void MenuList::removeMenuItem(const MenuItem * inItem)
    {
        NativeMenuList * menuList = static_cast<NativeMenuList *>(nativeComponent().get());
        menuList->removeMenuItem(inItem->getAttribute("label"));
    }

    
    MenuPopup::MenuPopup(const Type & inType, ElementPtr inParent) :
        Element(inType, inParent, gNullNativeComponent)
    {
    }


    void MenuPopup::addMenuItem(const MenuItem * inItem)
    {
        if (ElementPtr parent = mParent.lock())
        {
            if (parent->type() == eltype("menulist"))
            {
                static_cast<MenuList*>(parent.get())->addMenuItem(inItem);
            }
            else
            {
                ReportError("MenuPopup is located in non-compatible container.");
            }
        }
    }


    void MenuPopup::removeMenuItem(const MenuItem * inItem)
    {
        if (ElementPtr parent = mParent.lock())
        {
            if (parent->type() == eltype("menulist"))
            {
                static_cast<MenuList*>(parent.get())->removeMenuItem(inItem);
            }
            else
            {
                ReportError("MenuPopup is located in non-compatible container.");
            }
        }
    }

    
    MenuItem::MenuItem(const Type & inType, ElementPtr inParent) :
        Element(inType, inParent, gNullNativeComponent)
    {
    }
        
    
    MenuItem::~MenuItem()
    {
        if (ElementPtr parent = mParent.lock())
        {
            if (parent->type() == eltype("menupopup"))
            {
                static_cast<MenuPopup*>(parent.get())->removeMenuItem(this);
            }
            else
            {
                ReportError("MenuItem is located in non-compatible container.");
            }
        }
    }


    void MenuItem::init()
    {
        if (ElementPtr parent = mParent.lock())
        {
            if (parent->type() == eltype("menupopup"))
            {
                static_cast<MenuPopup*>(parent.get())->addMenuItem(this);
            }
            else
            {
                ReportError("MenuItem is located in non-compatible container.");
            }
        }
    }


} // XULWin
