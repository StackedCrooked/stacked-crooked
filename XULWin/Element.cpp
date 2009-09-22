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
                // ignore error reports about failure to apply attributes
                // it's unlikely to be an issue here
                ErrorCatcher errorIgnorer;
                errorIgnorer.disableLogging(true);
                setAttribute(it->first, it->second);
            }
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
        if (!mNativeComponent->setAttribute(inName, inValue))
        {
            mAttributes[inName] = inValue;
        }
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


    std::string Label::value() const
    {
        return getAttribute("value");
    }


    Description::Description(ElementPtr inParent) :
        Element(Description::Type(),
                inParent,
                NativeComponentPtr(new NativeDescription(inParent->nativeComponent())))
    {
    }


    Text::Text(ElementPtr inParent) :
        Element(Text::Type(),
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
        NativeMenuList * nativeMenuList = static_cast<NativeMenuList *>(nativeComponent().get());
        nativeMenuList->addMenuItem(inItem->label());
    }
        
    
    void MenuList::removeMenuItem(const MenuItem * inItem)
    {
        NativeMenuList * nativeMenuList = static_cast<NativeMenuList *>(nativeComponent().get());
        nativeMenuList->removeMenuItem(inItem->label());
    }

    
    MenuPopup::MenuPopup(ElementPtr inParent) :
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

    
    std::string MenuItem::label() const
    {
        return getAttribute("label");
    }

    
    std::string MenuItem::value() const
    {
        return getAttribute("value");
    }


    Separator::Separator(ElementPtr inParent) :
        Element(Separator::Type(),
                inParent,
                NativeComponentPtr(new NativeSeparator(inParent->nativeComponent())))
    {
    }


    Separator::~Separator()
    {
    }


    MenuButton::MenuButton(ElementPtr inParent) :
        Element(MenuButton::Type(),
                inParent,
                NativeComponentPtr(new NativeMenuButton(inParent->nativeComponent())))
    {
    }


    MenuButton::~MenuButton()
    {
    }


    Grid::Grid(ElementPtr inParent) :
        Element(Grid::Type(),
                inParent,
                NativeComponentPtr(new NativeGrid(inParent->nativeComponent())))
    {
    }


    Grid::~Grid()
    {
    }


    Rows::Rows(ElementPtr inParent) :
        Element(Rows::Type(),
                inParent,
                NativeComponentPtr(new NativeRows(inParent->nativeComponent())))
    {
    }


    Rows::~Rows()
    {
    }


    Columns::Columns(ElementPtr inParent) :
        Element(Columns::Type(),
                inParent,
                NativeComponentPtr(new NativeColumns(inParent->nativeComponent())))
    {
    }


    Columns::~Columns()
    {
    }


    Row::Row(ElementPtr inParent) :
        Element(Row::Type(),
                inParent,
                NativeComponentPtr(new NativeRow(inParent->nativeComponent())))
    {
    }


    Row::~Row()
    {
    }


    Column::Column(ElementPtr inParent) :
        Element(Column::Type(),
                inParent,
                NativeComponentPtr(new NativeColumn(inParent->nativeComponent())))
    {
    }


    Column::~Column()
    {
    }


} // XULWin
