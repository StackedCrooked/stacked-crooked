#include "Element.h"
#include "NativeComponent.h"
#include "Utils/ErrorReporter.h"
#include <boost/bind.hpp>


using namespace Utils;


namespace XULWin
{
    NativeComponentPtr gNullNativeComponent;


    Element::Element(const std::string & inType, ElementPtr inParent, boost::shared_ptr<NativeComponent> inNativeComponent) :
        mType(inType),
        mParent(inParent ? inParent.get() : 0),
        mNativeComponent(inNativeComponent)
    {
        if (mNativeComponent)
        {
            mNativeComponent->setOwningElement(this);
        }
    }


    const std::string & Element::type() const
    {
        return mType;
    }


    void Element::addEventListener(const std::string & inEvent, const EventHandler & inEventHandler)
    {
        mEventHandlers[inEvent].push_back(inEventHandler);
    }

    
    ElementPtr Element::getElementById(const std::string & inID)
    {
        struct Helper
        {
            static ElementPtr findChild(const Children & inChildren, const std::string & inID)
            {
                ElementPtr result;
                for (size_t idx = 0; idx != inChildren.size(); ++idx)
                {
                    ElementPtr child = inChildren[idx];
                    if (child->getAttribute("id") == inID)
                    {
                        result = child;
                        break;
                    }
                    else
                    {
                        result = findChild(child->children(), inID);
                        if (result)
                        {
                            break;
                        }
                    }
                }
                return result;
            }
        };
        return Helper::findChild(children(), inID);
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


    Window::Window(const std::string & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeWindow))
    {
    }


    void Window::showModal()
    {
        static_cast<NativeWindow *>(nativeComponent().get())->showModal();
    }


    Button::Button(const std::string & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeButton(inParent->nativeComponent())))
    {
    }


    Label::Label(const std::string & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeLabel(inParent->nativeComponent())))
    {
    }


    TextBox::TextBox(const std::string & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeTextBox(inParent->nativeComponent())))
    {
    }


    CheckBox::CheckBox(const std::string & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeCheckBox(inParent->nativeComponent())))
    {
    }


    Box::Box(const std::string & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeBox(inParent->nativeComponent())))
    {
    }


    HBox::HBox(const std::string & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeHBox(inParent->nativeComponent())))
    {
    }


    VBox::VBox(const std::string & inType, ElementPtr inParent) :
        Element(inType,
                inParent,
                NativeComponentPtr(new NativeVBox(inParent->nativeComponent())))
    {
    }


    MenuList::MenuList(const std::string & inType, ElementPtr inParent) :
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

    
    MenuPopup::MenuPopup(const std::string & inType, ElementPtr inParent) :
        Element(inType, inParent, gNullNativeComponent)
    {
    }


    void MenuPopup::addMenuItem(const MenuItem * inItem)
    {
        if (mParent->type() == "menulist")
        {
            static_cast<MenuList*>(mParent)->addMenuItem(inItem);
        }
        else
        {
            ReportError("MenuPopup is located in non-compatible container.");
        }
    }


    void MenuPopup::removeMenuItem(const MenuItem * inItem)
    {
        if (mParent->type() == "menulist")
        {
            static_cast<MenuList*>(mParent)->removeMenuItem(inItem);
        }
        else
        {
            ReportError("MenuPopup is located in non-compatible container.");
        }
    }

    
    MenuItem::MenuItem(const std::string & inType, ElementPtr inParent) :
        Element(inType, inParent, gNullNativeComponent)
    {
    }
        
    
    MenuItem::~MenuItem()
    {
        if (mParent->type() == "menupopup")
        {
            static_cast<MenuPopup*>(mParent)->removeMenuItem(this);
        }
        else
        {
            ReportError("MenuItem is located in non-compatible container.");
        }
    }


    void MenuItem::init()
    {
        if (mParent->type() == "menupopup")
        {
            static_cast<MenuPopup*>(mParent)->addMenuItem(this);
        }
        else
        {
            ReportError("MenuItem is located in non-compatible container.");
        }
    }


} // XULWin
