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


    Element::~Element()
    {
        // Children require parent access while destructing.
        // So we destruct them while parent still alive.
        mChildren.clear();
    }


    const std::string & Element::type() const
    {
        return mType;
    }


    void Element::handleEvent(const std::string & inEvent)
    {
        EventHandlers::iterator it = mEventHandlers.find(inEvent);
        if (it != mEventHandlers.end())
        {
            std::vector<EventHandler> & handlers = it->second;
            for (size_t idx = 0; idx != handlers.size(); ++idx)
            {
                handlers[idx](0);
            }
        }
    }

    
    ElementPtr Element::getElementById(const std::string & inId)
    {
        struct Helper
        {
            static ElementPtr findChildById(const Children & inChildren, const std::string & inId)
            {
                ElementPtr result;
                for (size_t idx = 0; idx != inChildren.size(); ++idx)
                {
                    ElementPtr child = inChildren[idx];
                    if (child->getAttribute("id") == inId)
                    {
                        result = child;
                        break;
                    }
                    else
                    {
                        result = findChildById(child->children(), inId);
                        if (result)
                        {
                            break;
                        }
                    }
                }
                return result;
            }
        };
        return Helper::findChildById(children(), inId);
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

        static std::string fNotFound;
        return fNotFound;
    }
    
    
    boost::shared_ptr<NativeComponent> Element::nativeComponent() const
    {
        return mNativeComponent;
    }
    
    
    void Element::addChild(ElementPtr inChild)
    {
        mChildren.push_back(inChild);
    }


    Window::Window(ElementPtr inParent) :
        Element(Window::Type(),
                inParent,
                NativeComponentPtr(new NativeWindow(gNullNativeComponent)))
    {
    }


    void Window::showModal()
    {
        static_cast<NativeWindow *>(nativeComponent().get())->showModal();
    }


    Button::Button(ElementPtr inParent) :
        Element(Button::Type(),
                inParent,
                NativeComponentPtr(new NativeButton(inParent->nativeComponent())))
    {
    }


    Label::Label(ElementPtr inParent) :
        Element(Label::Type(),
                inParent,
                NativeComponentPtr(new NativeLabel(inParent->nativeComponent())))
    {
    }


    TextBox::TextBox(ElementPtr inParent) :
        Element(TextBox::Type(),
                inParent,
                NativeComponentPtr(new NativeTextBox(inParent->nativeComponent())))
    {
    }


    CheckBox::CheckBox(ElementPtr inParent) :
        Element(CheckBox::Type(),
                inParent,
                NativeComponentPtr(new NativeCheckBox(inParent->nativeComponent())))
    {
    }


    Box::Box(ElementPtr inParent) :
        Element(Box::Type(),
                inParent,
                NativeComponentPtr(new NativeBox(inParent->nativeComponent())))
    {
    }


    HBox::HBox(ElementPtr inParent) :
        Element(HBox::Type(),
                inParent,
                NativeComponentPtr(new NativeHBox(inParent->nativeComponent())))
    {
    }


    VBox::VBox(ElementPtr inParent) :
        Element(VBox::Type(),
                inParent,
                NativeComponentPtr(new NativeVBox(inParent->nativeComponent())))
    {
    }


    MenuList::MenuList(ElementPtr inParent) :
        Element(MenuList::Type(),
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

    
    MenuPopup::MenuPopup(ElementPtr inParent) :
        Element(MenuPopup::Type(), inParent, gNullNativeComponent)
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

    
    MenuItem::MenuItem(ElementPtr inParent) :
        Element(MenuItem::Type(), inParent, gNullNativeComponent)
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
