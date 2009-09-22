#include "Element.h"
#include "NativeComponent.h"
#include "Utils/ErrorReporter.h"
#include <boost/bind.hpp>


using namespace Utils;


namespace XULWin
{
    NativeComponent * gNullNativeComponent(0);


    Element::Element(const std::string & inType, Element * inParent, NativeComponent * inNativeComponent) :
        mType(inType),
        mParent(inParent),
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


    std::string Element::label() const
    {
        return getAttribute("label");
    }

    
    void Element::setLabel(const std::string & inLabel)
    {
        setAttribute("label", inLabel);
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

    
    Element * Element::getElementById(const std::string & inId)
    {
        struct Helper
        {
            static Element * findChildById(const Children & inChildren, const std::string & inId)
            {
                Element * result(0);
                for (size_t idx = 0; idx != inChildren.size(); ++idx)
                {
                    ElementPtr child = inChildren[idx];
                    if (child->getAttribute("id") == inId)
                    {
                        result = child.get();
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
                // ignore error reports about failure to apply attributes
                // it's unlikely to be an issue here
                ErrorCatcher errorIgnorer;
                errorIgnorer.disableLogging(true);
                setAttribute(it->first, it->second);
            }
        }
    }


    void Element::setAttributeControllers()
    {
        if (mNativeComponent)
        {
            mNativeComponent->setAttributeControllers();
        }
    }

    
    std::string Element::getAttribute(const std::string & inName) const
    {
        std::string result;
        if (!mNativeComponent || !mNativeComponent->getAttribute(inName, result))
        {
            AttributesMapping::const_iterator it = mAttributes.find(inName);
            if (it != mAttributes.end())
            {
                result = it->second;
            }
        }
        return result;
    }
    
    
    void Element::setAttribute(const std::string & inName, const std::string & inValue)
    {
        if (!mNativeComponent || !mNativeComponent->setAttribute(inName, inValue))
        {
            mAttributes[inName] = inValue;
        }
    }
    
    
    NativeComponent * Element::nativeComponent() const
    {
        return mNativeComponent.get();
    }
    
    
    void Element::addChild(ElementPtr inChild)
    {
        mChildren.push_back(inChild);
    }


    Window::Window(Element * inParent) :
        Element(Window::Type(),
                inParent,
                new NativeWindow(gNullNativeComponent))
    {
    }


    void Window::showModal()
    {
        static_cast<NativeWindow *>(nativeComponent())->showModal();
    }


    Button::Button(Element * inParent) :
        Element(Button::Type(),
                inParent,
                new VirtualPadding(new NativeButton(inParent->nativeComponent())))
    {
    }


    Label::Label(Element * inParent) :
        Element(Label::Type(),
                inParent,
                new VirtualPadding(new NativeLabel(inParent->nativeComponent())))
    {
    }


    std::string Label::value() const
    {
        return getAttribute("value");
    }


    Description::Description(Element * inParent) :
        Element(Description::Type(),
                inParent,
                new VirtualPadding(new NativeDescription(inParent->nativeComponent())))
    {
    }


    Text::Text(Element * inParent) :
        Element(Text::Type(),
                inParent,
                new VirtualPadding(new NativeLabel(inParent->nativeComponent())))
    {
    }


    TextBox::TextBox(Element * inParent) :
        Element(TextBox::Type(),
                inParent,
                new VirtualPadding(new NativeTextBox(inParent->nativeComponent())))
    {
    }


    CheckBox::CheckBox(Element * inParent) :
        Element(CheckBox::Type(),
                inParent,
                new VirtualPadding(new NativeCheckBox(inParent->nativeComponent())))
    {
    }


    Box::Box(Element * inParent) :
        Element(Box::Type(),
                inParent,
                new NativeBox(inParent->nativeComponent()))
    {
    }


    HBox::HBox(Element * inParent) :
        Element(HBox::Type(),
                inParent,
                new NativeHBox(inParent->nativeComponent()))
    {
    }


    VBox::VBox(Element * inParent) :
        Element(VBox::Type(),
                inParent,
                new NativeVBox(inParent->nativeComponent()))
    {
    }


    MenuList::MenuList(Element * inParent) :
        Element(MenuList::Type(),
                inParent,
                new VirtualPadding(new NativeMenuList(inParent->nativeComponent())))
    {
    }
        
    
    void MenuList::addMenuItem(const MenuItem * inItem)
    {
        NativeMenuList * nativeMenuList = static_cast<NativeMenuList *>(nativeComponent());
        nativeMenuList->addMenuItem(inItem->label());
    }
        
    
    void MenuList::removeMenuItem(const MenuItem * inItem)
    {
        NativeMenuList * nativeMenuList = static_cast<NativeMenuList *>(nativeComponent());
        nativeMenuList->removeMenuItem(inItem->label());
    }

    
    MenuPopup::MenuPopup(Element * inParent) :
        Element(MenuPopup::Type(), inParent, gNullNativeComponent),
        mDestructing(false)
    {
    }

    
    MenuPopup::~MenuPopup()
    {
        mDestructing = true;
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
        if (mDestructing)
        {
            return;
        }

        if (mParent->type() == "menulist")
        {
            static_cast<MenuList*>(mParent)->removeMenuItem(inItem);
        }
        else
        {
            ReportError("MenuPopup is located in non-compatible container.");
        }
    }

    
    MenuItem::MenuItem(Element * inParent) :
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

    
    std::string MenuItem::label() const
    {
        return getAttribute("label");
    }

    
    std::string MenuItem::value() const
    {
        return getAttribute("value");
    }


    Separator::Separator(Element * inParent) :
        Element(Separator::Type(),
                inParent,
                new VirtualPadding(new NativeSeparator(inParent->nativeComponent())))
    {
    }


    Separator::~Separator()
    {
    }


    Spacer::Spacer(Element * inParent) :
        Element(Spacer::Type(),
                inParent,
                new NativeSpacer(inParent->nativeComponent()))
    {
    }


    Spacer::~Spacer()
    {
    }


    MenuButton::MenuButton(Element * inParent) :
        Element(MenuButton::Type(),
                inParent,
                new VirtualPadding(new NativeMenuButton(inParent->nativeComponent())))
    {
    }


    MenuButton::~MenuButton()
    {
    }


    Grid::Grid(Element * inParent) :
        Element(Grid::Type(),
                inParent,
                new NativeGrid(inParent->nativeComponent()))
    {
    }


    Grid::~Grid()
    {
    }


    Rows::Rows(Element * inParent) :
        Element(Rows::Type(),
                inParent,
                new NativeRows(inParent->nativeComponent()))
    {
    }


    Rows::~Rows()
    {
    }


    Columns::Columns(Element * inParent) :
        Element(Columns::Type(),
                inParent,
                new NativeColumns(inParent->nativeComponent()))
    {
    }


    Columns::~Columns()
    {
    }


    Row::Row(Element * inParent) :
        Element(Row::Type(),
                inParent,
                new NativeRow(inParent->nativeComponent()))
    {
    }


    Row::~Row()
    {
    }


    Column::Column(Element * inParent) :
        Element(Column::Type(),
                inParent,
                new NativeColumn(inParent->nativeComponent()))
    {
    }


    Column::~Column()
    {
    }


} // XULWin
