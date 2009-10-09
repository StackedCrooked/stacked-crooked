#include "Element.h"
#include "ElementCreationSupport.h"
#include "ElementFactory.h"
#include "Decorator.h"
#include "Defaults.h"
#include "ElementImpl.h"
#include "ToolbarCustomWindowDecorator.h"
#include "Utils/ErrorReporter.h"
#include <boost/bind.hpp>


using namespace Utils;


namespace XULWin
{

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

    
    bool Element::init()
    {
        return impl()->initImpl();
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
            Poco::StringTokenizer keyValuePairs(it->second, ";",
                                                Poco::StringTokenizer::TOK_IGNORE_EMPTY
                                                | Poco::StringTokenizer::TOK_TRIM);

            Poco::StringTokenizer::Iterator it = keyValuePairs.begin(), end = keyValuePairs.end();
            for (; it != end; ++it)
            {
                std::string::size_type sep = it->find(":");
                if (sep != std::string::npos && (sep + 1) < it->size())
                {
                    std::string key = it->substr(0, sep);
                    std::string value = it->substr(sep + 1, it->size() - sep - 1);
                    setStyle(key, value);
                }
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
                new MarginDecorator(CreateNativeControl<NativeButton>(inParent, inAttributesMapping)))
    {
    }


    Label::Label(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Label::Type(),
                inParent,
                new MarginDecorator(CreateNativeControl<NativeLabel>(inParent, inAttributesMapping)))
    {
    }


    Description::Description(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Description::Type(),
                inParent,
                new MarginDecorator(CreateNativeControl<NativeDescription>(inParent, inAttributesMapping)))
    {
    }


    bool Description::init()
    {
        setAttribute("value", innerText());
        return Element::init();
    }


    Text::Text(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Text::Type(),
                inParent,
                new MarginDecorator(CreateNativeControl<NativeLabel>(inParent, inAttributesMapping)))
    {
    }


    TextBox::TextBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TextBox::Type(),
                inParent,
                new MarginDecorator(CreateNativeControl<NativeTextBox>(inParent, inAttributesMapping)))
    {
    }


    CheckBox::CheckBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(CheckBox::Type(),
                inParent,
                new MarginDecorator(CreateNativeControl<NativeCheckBox>(inParent, inAttributesMapping)))
    {
    }


    Box::Box(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Box::Type(),
                inParent,
                CreateContainer<VirtualBox, NativeBox>(inParent, inAttributesMapping))
    {
    }


    HBox::HBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(HBox::Type(),
                inParent,
                CreateContainer<VirtualBox, NativeBox>(inParent, inAttributesMapping))
    {
        impl()->setOrient(Horizontal);
    }


    VBox::VBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(VBox::Type(),
                inParent,CreateContainer<VirtualBox, NativeBox>(inParent, inAttributesMapping))
    {
        impl()->setOrient(Vertical);
    }


    MenuList::MenuList(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(MenuList::Type(),
                inParent,
                new MarginDecorator(CreateNativeControl<NativeMenuList>(inParent, inAttributesMapping)))
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
        Element(MenuPopup::Type(),
                inParent,
                new PassiveComponent(inParent->impl(), inAttributesMapping)),
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
        Element(MenuItem::Type(),
                inParent,
                new PassiveComponent(inParent->impl(), inAttributesMapping))
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


    bool MenuItem::init()
    {
        if (MenuPopup * popup = mParent->downcast<MenuPopup>())
        {
            popup->addMenuItem(this);
        }
        else
        {
            ReportError("MenuItem is located in non-compatible container.");
        }
        return Element::init();
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
                new MarginDecorator(CreateNativeControl<NativeSeparator>(inParent, inAttributesMapping)))
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


    Grid::Grid(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Grid::Type(),
                inParent,
                CreateContainer<VirtualGrid, NativeGrid>(inParent, inAttributesMapping))
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


    TabBox::TabBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TabBox::Type(),
                inParent,
                CreateContainer<VirtualBox, NativeBox>(inParent, inAttributesMapping))
    { 
    }


    TabBox::~TabBox()
    {
    }


    Tabs::Tabs(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Tabs::Type(),
                inParent,
                new TabsImpl(inParent->impl(), inAttributesMapping))
    {
    }


    Tabs::~Tabs()
    {
    }


    Tab::Tab(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Tab::Type(),
                inParent,
                new TabImpl(inParent->impl(), inAttributesMapping))
    {
    }


    Tab::~Tab()
    {
    }


    TabPanels::TabPanels(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TabPanels::Type(),
                inParent,
                new TabPanelsImpl(inParent->impl(), inAttributesMapping))
    { 
    }


    TabPanels::~TabPanels()
    {
    }


    TabPanel::TabPanel(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TabPanel::Type(),
                inParent,
                new TabPanelImpl(inParent->impl(), inAttributesMapping))
    { 
    }


    TabPanel::~TabPanel()
    {
    }


    GroupBox::GroupBox(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(GroupBox::Type(),
                inParent,
                new MarginDecorator(new GroupBoxImpl(inParent->impl(), inAttributesMapping)))
    { 
    }


    Caption::Caption(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Caption::Type(),
                inParent,
                new CaptionImpl(inParent->impl(), inAttributesMapping))
    {
    }


    Tree::Tree(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Tree::Type(),
                inParent,
                new TreeImpl(inParent->impl(), inAttributesMapping))
    {
    }


    Tree::~Tree()
    {
    }


    TreeChildren::TreeChildren(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeChildren::Type(),
                inParent,
                new TreeChildrenImpl(inParent->impl(), inAttributesMapping))
    {
    }


    TreeChildren::~TreeChildren()
    {
    }


    TreeItem::TreeItem(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeItem::Type(),
                inParent,
                new TreeItemImpl(inParent->impl(), inAttributesMapping))
    {
    }


    TreeItem::~TreeItem()
    {
    }


    TreeCols::TreeCols(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeCols::Type(),
                inParent,
                new TreeColsImpl(inParent->impl(), inAttributesMapping))
    {
    }


    TreeCols::~TreeCols()
    {
    }


    TreeCol::TreeCol(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeCol::Type(),
                inParent,
                new TreeColImpl(inParent->impl(), inAttributesMapping))
    {
    }


    TreeCol::~TreeCol()
    {
    }    
    
    
    TreeRow::TreeRow(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeRow::Type(),
                inParent,
                new TreeRowImpl(inParent->impl(), inAttributesMapping))
    {
    }


    TreeRow::~TreeRow()
    {
    }

    
    TreeCell::TreeCell(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeCell::Type(),
                inParent,
                new TreeCellImpl(inParent->impl(), inAttributesMapping))
    {
    }


    TreeCell::~TreeCell()
    {
    }

    
    Statusbar::Statusbar(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Statusbar::Type(),
                inParent,
                new StatusbarImpl(inParent->impl(), inAttributesMapping))
    {
    }


    Statusbar::~Statusbar()
    {
    }

    
    StatusbarPanel::StatusbarPanel(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(StatusbarPanel::Type(),
                inParent,
                new StatusbarPanelImpl(inParent->impl(), inAttributesMapping))
    {
    }


    StatusbarPanel::~StatusbarPanel()
    {
    }

    
    Toolbar::Toolbar(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Toolbar::Type(),
                inParent,
                new ToolbarImpl(inParent->impl(), inAttributesMapping))
    {
    }


    Toolbar::~Toolbar()
    {
    }

    
    ToolbarButton::ToolbarButton(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ToolbarButton::Type(),
                inParent,
                new ToolbarButtonImpl(inParent->impl(), inAttributesMapping))
    {
    }


    ToolbarButton::~ToolbarButton()
    {
    }


} // XULWin
