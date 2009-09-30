#include "Element.h"
#include "Decorator.h"
#include "Defaults.h"
#include "ElementImpl.h"
#include "Utils/ErrorReporter.h"
#include "Poco/StringTokenizer.h"
#include <boost/bind.hpp>


using namespace Utils;


namespace XULWin
{
    ElementImpl * gNullNativeComponent(0);


    Element::Element(const std::string & inType, Element * inParent, ElementImpl * inNativeComponent) :
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


    void Element::setInnerText(const std::string & inText)
    {
        mInnerText = inText;
    }


    const std::string & Element::innerText() const
    {
        return mInnerText;
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


    void Element::setStyles(const AttributesMapping & inAttributes)
    {
        AttributesMapping::const_iterator it = inAttributes.find("style");
        if (it != inAttributes.end())
        {
            Poco::StringTokenizer tok(it->second, ";:",
                                      Poco::StringTokenizer::TOK_IGNORE_EMPTY
                                      | Poco::StringTokenizer::TOK_TRIM);

            Poco::StringTokenizer::Iterator it = tok.begin(), end = tok.end();
            std::string key, value;
            int counter = 0;
            for (; it != end; ++it)
            {
                if (counter%2 == 0)
                {
                    key = *it;
                }
                else
                {
                    value = *it;
                    setStyle(key, value);
                }
                counter++;
            }
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


    void Element::initStyleControllers()
    {
        if (mImpl)
        {
            mImpl->initStyleControllers();
        }
    }

    
    std::string Element::getStyle(const std::string & inName) const
    {
        std::string result;
        if (!mImpl || !mImpl->getStyle(inName, result))
        {
            StylesMapping::const_iterator it = mStyles.find(inName);
            if (it != mStyles.end())
            {
                result = it->second;
            }
        }
        return result;

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
    
    
    std::string Element::getDocumentAttribute(const std::string & inName) const
    {
        std::string result;
        AttributesMapping::const_iterator it = mAttributes.find(inName);
        if (it != mAttributes.end())
        {
            result = it->second;
        }
        return result;
    }
    
    
    void Element::setStyle(const std::string & inName, const std::string & inValue)
    {
        std::string type = this->type();
        if (!mImpl || !mImpl->setStyle(inName, inValue))
        {
            mStyles[inName] = inValue;
        }
    }
    
    
    void Element::setAttribute(const std::string & inName, const std::string & inValue)
    {
        if (!mImpl || !mImpl->setAttribute(inName, inValue))
        {
            mAttributes[inName] = inValue;
        }
    }
    
    
    bool Element::addEventListener(EventListener * inEventListener)
    {
        if (!mImpl)
        {
            return false;
        }

        if (NativeComponent * comp = mImpl->downcast<NativeComponent>())
        {
            comp->addEventListener(inEventListener);
            return true;
        }
        return false;
    }


    bool Element::removeEventListener(EventListener * inEventListener)
    {
        if (!mImpl)
        {
            return false;
        }

        if (NativeComponent * comp = mImpl->downcast<NativeComponent>())
        {
            comp->removeEventListener(inEventListener);
            return true;
        }
        return false;
    }
    
    
    ElementImpl * Element::impl() const
    {
        return mImpl.get();
    }
    
    
    void Element::addChild(ElementPtr inChild)
    {
        mChildren.push_back(inChild);
    }


    Window::Window(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Window::Type(),
                inParent,
                new NativeWindow(inAttributesMapping))
    {
    }


    void Window::showModal()
    {
        if (NativeWindow * nativeWindow = impl()->downcast<NativeWindow>())
        {
            nativeWindow->showModal();
        }
    }


    Button::Button(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Button::Type(),
                inParent,
                new MarginDecorator(new NativeButton(inParent->impl(), inAttributesMapping)))
    {
    }


    Label::Label(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Label::Type(),
                inParent,
                new MarginDecorator(new NativeLabel(inParent->impl(), inAttributesMapping)))
    {
    }


    std::string Label::value() const
    {
        return getAttribute("value");
    }


    Description::Description(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Description::Type(),
                inParent,
                new MarginDecorator(new NativeDescription(inParent->impl(), inAttributesMapping)))
    {
    }


    void Description::init()
    {
        setAttribute("value", innerText());
    }


    Text::Text(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Text::Type(),
                inParent,
                new MarginDecorator(new NativeLabel(inParent->impl(), inAttributesMapping)))
    {
    }


    TextBox::TextBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TextBox::Type(),
                inParent,
                new MarginDecorator(new NativeTextBox(inParent->impl(), inAttributesMapping)))
    {
    }


    CheckBox::CheckBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(CheckBox::Type(),
                inParent,
                new MarginDecorator(new NativeCheckBox(inParent->impl(), inAttributesMapping)))
    {
    }


    void GetStyles(const AttributesMapping & inAttributesMapping, StylesMapping & styles)
    {        
        StylesMapping::const_iterator it = inAttributesMapping.find("style");
        if (it != inAttributesMapping.end())
        {
            Poco::StringTokenizer tok(it->second, ";:", Poco::StringTokenizer::TOK_IGNORE_EMPTY | Poco::StringTokenizer::TOK_TRIM);
            Poco::StringTokenizer::Iterator it = tok.begin(), end = tok.end();
            std::string key, value;
            int counter = 0;
            for (; it != end; ++it)
            {
                if (counter%2 == 0)
                {
                    key = *it;
                }
                else
                {
                    value = *it;
                    styles.insert(std::make_pair(key, value));
                }
                counter++;
            }
        }
    }


    static ElementImpl * CreateBox(Element * inParent,
                                   const AttributesMapping & inAttributesMapping,
                                   Orient inOrient = Defaults::Attributes::orient(),
                                   Align inAlign = Defaults::Attributes::align())
    {
        struct Helper
        {
            static bool Has(const StylesMapping & inStylesMapping, const std::string & value)
            {
                return inStylesMapping.find(value) != inStylesMapping.end();
            }
        };
        StylesMapping styles;
        GetStyles(inAttributesMapping, styles);
        bool overflow = Helper::Has(styles, "overflow");
        bool overflowX = overflow || Helper::Has(styles, "overflow-x");
        bool overflowY = overflow || Helper::Has(styles, "overflow-y");
        if (overflowX || overflowY)
        {
            ScrollDecorator * result;
            NativeBox * box = new NativeBox(inParent->impl(), inAttributesMapping, inOrient, inAlign);
            if (overflowX && !overflowY)
            {
                result = new ScrollDecorator(inParent->impl(), box, ScrollDecorator::Horizontal);
            }
            else if (overflowY && !overflowX)
            {
                result = new ScrollDecorator(inParent->impl(), box, ScrollDecorator::Vertical);
            }
            else
            {
                result = new ScrollDecorator(inParent->impl(), box, ScrollDecorator::Both);
            }
            return result;
        }
        else
        {
            return new Decorator(new VirtualBox(inParent->impl(), inAttributesMapping, inOrient, inAlign));
        }
    }


    Box::Box(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Box::Type(),
                inParent,
                CreateBox(inParent, inAttributesMapping))
    {
    }


    HBox::HBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(HBox::Type(),
                inParent,
                CreateBox(inParent, inAttributesMapping, HORIZONTAL))
    {
    }


    VBox::VBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(VBox::Type(),
                inParent,
                CreateBox(inParent, inAttributesMapping, VERTICAL))
    {
    }


    MenuList::MenuList(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(MenuList::Type(),
                inParent,
                new MarginDecorator(new NativeMenuList(inParent->impl(), inAttributesMapping)))
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

    
    MenuPopup::MenuPopup(Element * inParent, const AttributesMapping & inAttributesMapping) :
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

    
    MenuItem::MenuItem(Element * inParent, const AttributesMapping & inAttributesMapping) :
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


    Separator::Separator(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Separator::Type(),
                inParent,
                new MarginDecorator(new NativeSeparator(inParent->impl(), inAttributesMapping)))
    {
    }


    Separator::~Separator()
    {
    }


    Spacer::Spacer(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Spacer::Type(),
                inParent,
                new NativeSpacer(inParent->impl(), inAttributesMapping))
    {
    }


    Spacer::~Spacer()
    {
    }


    MenuButton::MenuButton(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(MenuButton::Type(),
                inParent,
                new MarginDecorator(new NativeMenuButton(inParent->impl(), inAttributesMapping)))
    {
    }


    MenuButton::~MenuButton()
    {
    }





    static ElementImpl * CreateGrid(Element * inParent,
                                   const AttributesMapping & inAttributesMapping)
    {
        struct Helper
        {
            static bool Has(const StylesMapping & inStylesMapping, const std::string & value)
            {
                return inStylesMapping.find(value) != inStylesMapping.end();
            }
        };
        StylesMapping styles;
        GetStyles(inAttributesMapping, styles);
        bool overflow = Helper::Has(styles, "overflow");
        bool overflowX = overflow || Helper::Has(styles, "overflow-x");
        bool overflowY = overflow || Helper::Has(styles, "overflow-y");
        if (overflowX || overflowY)
        {
            ScrollDecorator * result;
            NativeGrid * grid = new NativeGrid(inParent->impl(), inAttributesMapping);
            if (overflowX && !overflowY)
            {
                result = new ScrollDecorator(inParent->impl(), grid, ScrollDecorator::Horizontal);
            }
            if (overflowY && !overflowX)
            {
                result = new ScrollDecorator(inParent->impl(), grid, ScrollDecorator::Vertical);
            }
            else
            {
                result = new ScrollDecorator(inParent->impl(), grid, ScrollDecorator::Both);
            }
            return result;
        }
        else
        {
            return new Decorator(new VirtualGrid(inParent->impl(), inAttributesMapping));
        }
    }


    Grid::Grid(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Grid::Type(),
                inParent,
                CreateGrid(inParent, inAttributesMapping))
    {
    }


    Grid::~Grid()
    {
    }


    Rows::Rows(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Rows::Type(),
                inParent,
                new NativeRows(inParent->impl(), inAttributesMapping))
    {
    }


    Rows::~Rows()
    {
    }


    Columns::Columns(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Columns::Type(),
                inParent,
                new NativeColumns(inParent->impl(), inAttributesMapping))
    {
    }


    Columns::~Columns()
    {
    }


    Row::Row(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Row::Type(),
                inParent,
                new NativeRow(inParent->impl(), inAttributesMapping))
    {
    }


    Row::~Row()
    {
    }


    Column::Column(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Column::Type(),
                inParent,
                new NativeColumn(inParent->impl(), inAttributesMapping))
    {
    }


    Column::~Column()
    {
    }


    RadioGroup::RadioGroup(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(RadioGroup::Type(),
                inParent,
                new Decorator(new NativeRadioGroup(inParent->impl(), inAttributesMapping)))
    {
    }


    RadioGroup::~RadioGroup()
    {
    }


    Radio::Radio(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Radio::Type(),
                inParent,
                new MarginDecorator(new NativeRadio(inParent->impl(), inAttributesMapping)))
    {
    }


    Radio::~Radio()
    {
    }


    ProgressMeter::ProgressMeter(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ProgressMeter::Type(),
                inParent,
                new MarginDecorator(new NativeProgressMeter(inParent->impl(), inAttributesMapping)))
    {
    }


    ProgressMeter::~ProgressMeter()
    {
    }


    Deck::Deck(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Deck::Type(),
                inParent,
                new Decorator(new NativeDeck(inParent->impl(), inAttributesMapping)))
    {
    }


    Deck::~Deck()
    {
    }


    Scrollbar::Scrollbar(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Scrollbar::Type(),
                inParent,
                new Decorator(new NativeScrollbar(inParent->impl(), inAttributesMapping)))
    {
    }


    Scrollbar::~Scrollbar()
    {
    }


} // XULWin
