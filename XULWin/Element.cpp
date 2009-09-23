#include "Element.h"
#include "NativeComponent.h"
#include "Utils/ErrorReporter.h"
#include <boost/bind.hpp>


using namespace Utils;


namespace XULWin
{
    NativeElement * gNullNativeComponent(0);


    Element::Element(const std::string & inType, Element * inParent, NativeElement * inNativeComponent) :
        mType(inType),
        mParent(inParent),
        mImpl(inNativeComponent)
    {
        if (mImpl)
        {
            mImpl->setOwningElement(this);
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
            std::vector<EventHandler*> & handlers = it->second;
            for (size_t idx = 0; idx != handlers.size(); ++idx)
            {
                handlers[idx]->handle(0);
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
            mImpl->rebuildLayout();
        }
        else
        {
            ReportError("Remove child failed because it wasn't found.");
        }
    }


    void Element::setAttributes(const AttributesMapping & inAttributes)
    {
        mAttributes = inAttributes;
        
        if (mImpl)
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


    void Element::initAttributeControllers()
    {
        if (mImpl)
        {
            mImpl->initAttributeControllers();
        }
    }

    
    std::string Element::getAttribute(const std::string & inName) const
    {
        std::string result;
        if (!mImpl || !mImpl->getAttribute(inName, result))
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
        if (!mImpl || !mImpl->setAttribute(inName, inValue))
        {
            mAttributes[inName] = inValue;
        }
    }
    
    
    NativeElement * Element::impl() const
    {
        return mImpl.get();
    }
    
    
    void Element::addChild(ElementPtr inChild)
    {
        mChildren.push_back(inChild);
    }


    Window::Window(Element * inParent) :
        Element(Window::Type(),
                inParent,
                new NativeWindow())
    {
    }


    void Window::showModal()
    {
        if (NativeWindow * nativeWindow = impl()->downcast<NativeWindow>())
        {
            nativeWindow->showModal();
        }
    }


    Button::Button(Element * inParent) :
        Element(Button::Type(),
                inParent,
                new PaddingDecorator(new NativeButton(inParent->impl())))
    {
    }


    Label::Label(Element * inParent) :
        Element(Label::Type(),
                inParent,
                new PaddingDecorator(new NativeLabel(inParent->impl())))
    {
    }


    std::string Label::value() const
    {
        return getAttribute("value");
    }


    Description::Description(Element * inParent) :
        Element(Description::Type(),
                inParent,
                new PaddingDecorator(new NativeDescription(inParent->impl())))
    {
    }


    Text::Text(Element * inParent) :
        Element(Text::Type(),
                inParent,
                new PaddingDecorator(new NativeLabel(inParent->impl())))
    {
    }


    TextBox::TextBox(Element * inParent) :
        Element(TextBox::Type(),
                inParent,
                new PaddingDecorator(new NativeTextBox(inParent->impl())))
    {
    }


    CheckBox::CheckBox(Element * inParent) :
        Element(CheckBox::Type(),
                inParent,
                new PaddingDecorator(new NativeCheckBox(inParent->impl())))
    {
    }


    Box::Box(Element * inParent) :
        Element(Box::Type(),
                inParent,
                new NativeBox(inParent->impl()))
    {
    }


    HBox::HBox(Element * inParent) :
        Element(HBox::Type(),
                inParent,
                new NativeHBox(inParent->impl()))
    {
    }


    VBox::VBox(Element * inParent) :
        Element(VBox::Type(),
                inParent,
                new NativeVBox(inParent->impl()))
    {
    }


    MenuList::MenuList(Element * inParent) :
        Element(MenuList::Type(),
                inParent,
                new PaddingDecorator(new NativeMenuList(inParent->impl())))
    {
    }
        
    
    void MenuList::addMenuItem(const MenuItem * inItem)
    {
        if (NativeMenuList * nativeMenuList = impl()->downcast<NativeMenuList>())
        {
            nativeMenuList->addMenuItem(inItem->label());
        }
    }
        
    
    void MenuList::removeMenuItem(const MenuItem * inItem)
    {
        if (NativeMenuList * nativeMenuList = impl()->downcast<NativeMenuList>())
        {
            nativeMenuList->removeMenuItem(inItem->label());
        }
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
            if (MenuList * menuList = mParent->downcast<MenuList>())
            {
                menuList->addMenuItem(inItem);
            }
        }
        else if (mParent->type() == "menubutton")
        {
            // not yet implemented
        }
        else
        {
            ReportError("MenuPopup is located in non-compatible container.");
        }
    }


    void MenuPopup::removeMenuItem(const MenuItem * inItem)
    {
        // We don't remove menu items when destructing.
        // It is not needed.
        if (mDestructing)
        {
            return;
        }

        if (mParent->type() == "menulist")
        {
            if (MenuList * menuList = mParent->downcast<MenuList>())
            {
                menuList->removeMenuItem(inItem);
            }
        }
        else if (mParent->type() == "menubutton")
        {
            // not yet implemented
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
        if (MenuPopup * popup = mParent->downcast<MenuPopup>())
        {
            popup->removeMenuItem(this);
        }
        else
        {
            ReportError("MenuItem is located in non-compatible container.");
        }
    }


    void MenuItem::init()
    {
        if (MenuPopup * popup = mParent->downcast<MenuPopup>())
        {
            popup->addMenuItem(this);
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
                new PaddingDecorator(new NativeSeparator(inParent->impl())))
    {
    }


    Separator::~Separator()
    {
    }


    Spacer::Spacer(Element * inParent) :
        Element(Spacer::Type(),
                inParent,
                new NativeSpacer(inParent->impl()))
    {
    }


    Spacer::~Spacer()
    {
    }


    MenuButton::MenuButton(Element * inParent) :
        Element(MenuButton::Type(),
                inParent,
                new PaddingDecorator(new NativeMenuButton(inParent->impl())))
    {
    }


    MenuButton::~MenuButton()
    {
    }


    Grid::Grid(Element * inParent) :
        Element(Grid::Type(),
                inParent,
                new NativeGrid(inParent->impl()))
    {
    }


    Grid::~Grid()
    {
    }


    Rows::Rows(Element * inParent) :
        Element(Rows::Type(),
                inParent,
                new NativeRows(inParent->impl()))
    {
    }


    Rows::~Rows()
    {
    }


    Columns::Columns(Element * inParent) :
        Element(Columns::Type(),
                inParent,
                new NativeColumns(inParent->impl()))
    {
    }


    Columns::~Columns()
    {
    }


    Row::Row(Element * inParent) :
        Element(Row::Type(),
                inParent,
                new NativeRow(inParent->impl()))
    {
    }


    Row::~Row()
    {
    }


    Column::Column(Element * inParent) :
        Element(Column::Type(),
                inParent,
                new NativeColumn(inParent->impl()))
    {
    }


    Column::~Column()
    {
    }


    RadioGroup::RadioGroup(Element * inParent) :
        Element(RadioGroup::Type(),
                inParent,
                new NativeRadioGroup(inParent->impl()))
    {
    }


    RadioGroup::~RadioGroup()
    {
    }


    Radio::Radio(Element * inParent) :
        Element(Radio::Type(),
                inParent,
                new PaddingDecorator(new NativeRadio(inParent->impl())))
    {
    }


    Radio::~Radio()
    {
    }


    ProgressMeter::ProgressMeter(Element * inParent) :
        Element(ProgressMeter::Type(),
                inParent,
                new PaddingDecorator(new NativeProgressMeter(inParent->impl())))
    {
    }


    ProgressMeter::~ProgressMeter()
    {
    }


} // XULWin
