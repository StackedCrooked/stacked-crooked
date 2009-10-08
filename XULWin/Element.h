#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED


#include "Enums.h"
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>


namespace XULWin
{

    class ElementFactory;
    class ElementImpl;
    class Element;
    typedef boost::shared_ptr<Element> ElementPtr;
    typedef std::vector<ElementPtr> Children;
    typedef std::map<std::string, std::string> AttributesMapping;
    typedef std::map<std::string, std::string> StylesMapping;

    class Event;
    class EventListener;

    /**
     * Represents a XUL element.
     */
    class Element : private boost::noncopyable
    {
    public:
        template<class T>
        static ElementPtr Create(Element * inParent,
                                 const AttributesMapping & inAttr)
        {
            ElementPtr result(new T(inParent, inAttr));
            if (inParent)
            {
                inParent->addChild(result);
            }
            result->initAttributeControllers();
            result->setAttributes(inAttr);
            result->initStyleControllers();
            result->setStyles(inAttr);
            return result;
        }

        ~Element();

        // called by parser at end-element event
        virtual bool init();

        const std::string & type() const;

        Element * parent() const { return mParent; }

        // Gets the attribute value from the attribute controller.
        // If no attribute controller was found then it will search
        // the attribute mapping.
        // Returns empty string if not found.
        std::string getAttribute(const std::string & inName) const;

        // Gets the attribute from the attribute mapping
        // without querying the attribute controllers.
        // Returns empty string if not found.
        std::string getDocumentAttribute(const std::string & inName) const;

        // Gets the CSS property from the style controller.
        // If no style controller was found then it will search
        // the style mapping.
        // Returns empty string if not found.
        std::string getStyle(const std::string & inName) const;

        // Sets the attribute by invoking the attribute controller.
        // If no attribute controller found it will insert (or overwrite)
        // the value in the attributes mapping.
        void setAttribute(const std::string & inName, const std::string & inValue);

        void setInnerText(const std::string & inText);

        const std::string & innerText() const;

        Element * getElementById(const std::string & inId);

        const Children & children() const { return mChildren; }

        bool addEventListener(EventListener * inEventListener);

        bool removeEventListener(EventListener * inEventListener);

        ElementImpl * impl() const;

        template<class ElementType>
        const ElementType * downcast() const
        {
            if (type() == ElementType::Type())
            {
                return static_cast<ElementType*>(this);
            }
            return 0;
        }

        template<class ElementType>
        ElementType * downcast()
        {
            if (type() == ElementType::Type())
            {
                return static_cast<ElementType*>(this);
            }
            return 0;
        }

        // you don't need to call this, the factory method takes care of it
        virtual void addChild(ElementPtr inChild);

        // highly volatile, use at your own risk
        virtual void removeChild(const Element * inChild);

    protected:
        Element(const std::string & inType, Element * inParent, ElementImpl * inNativeComponent);

        Element * mParent;
        Children mChildren;

    private:

        void setAttributes(const AttributesMapping & inAttributes);

        void setStyle(const std::string & inName, const std::string & inValue);

        void setStyles(const AttributesMapping & inAttributes);

        void initAttributeControllers();

        void initStyleControllers();

        friend class ElementFactory;
        std::string mType;
        AttributesMapping mAttributes;
        StylesMapping mStyles;
        std::string mInnerText;
        boost::shared_ptr<ElementImpl> mImpl;
    };

    class Window : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Window>(inParent, inAttr); }

        static const char * Type() { return "window"; }

        void showModal();

    private:
        friend class Element;
        Window(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Button : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Button>(inParent, inAttr); }

        static const char * Type() { return "button"; }
    
    private:
        friend class Element;
        Button(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Label : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Label>(inParent, inAttr); }

        static const char * Type() { return "label"; }
    
    private:
        friend class Element;
        Label(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Description : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Description>(inParent, inAttr); }

        static const char * Type() { return "description"; }

        virtual bool init();
    
    private:
        friend class Element;
        Description(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    // Text is the same as label
    class Text : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Text>(inParent, inAttr); }

        static const char * Type() { return "text"; }
    
    private:
        friend class Element;
        Text(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TextBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TextBox>(inParent, inAttr); }

        static const char * Type() { return "textbox"; }
    
    private:
        friend class Element;
        TextBox(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class CheckBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<CheckBox>(inParent, inAttr); }

        static const char * Type() { return "checkbox"; }

    private:
        friend class Element;
        CheckBox(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Box : public Element
    {
    public: 
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Box>(inParent, inAttr); }

        static const char * Type() { return "box"; }
    private:
        friend class Element;
        Box(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class HBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<HBox>(inParent, inAttr); }

        static const char * Type() { return "hbox"; }

    private:
        friend class Element;
        HBox(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class VBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<VBox>(inParent, inAttr); }

        static const char * Type() { return "vbox"; }

    private:
        friend class Element;
        VBox(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class MenuItem;
    class MenuList : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuList>(inParent, inAttr); }

        static const char * Type() { return "menulist"; }

        void addMenuItem(const MenuItem * inItem);

        void removeMenuItem(const MenuItem * inItem);

    private:
        friend class Element;
        MenuList(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class MenuPopup : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuPopup>(inParent, inAttr); }

        virtual ~MenuPopup();

        static const char * Type() { return "menupopup"; }

        void addMenuItem(const MenuItem * inItem);

        void removeMenuItem(const MenuItem * inItem);

    private:
        friend class Element;
        MenuPopup(Element * inParent, const AttributesMapping & inAttributesMapping);

        // This flag is set to true when destructor is called.
        // It is used to prevent that the destruction of
        // child menu items would trigger the 'removeMenuItem' call
        // which is not needed when destructing and it could
        // cause crashes is certain situations.
        bool mDestructing;
    };


    class MenuItem : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuItem>(inParent, inAttr); }

        static const char * Type() { return "menuitem"; }

        virtual ~MenuItem();

        virtual bool init();

        std::string label() const;

        std::string value() const;

    private:
        friend class Element;
        MenuItem(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Separator : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Separator>(inParent, inAttr); }

        static const char * Type() { return "separator"; }

        virtual ~Separator();

    private:
        friend class Element;
        Separator(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Spacer : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Spacer>(inParent, inAttr); }

        static const char * Type() { return "spacer"; }

        virtual ~Spacer();

    private:
        friend class Element;
        Spacer(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class MenuButton : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuButton>(inParent, inAttr); }

        static const char * Type() { return "menubutton"; }

        virtual ~MenuButton();

    private:
        friend class Element;
        MenuButton(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Rows;
    class Columns;
    class Grid : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Grid>(inParent, inAttr); }

        static const char * Type() { return "grid"; }

        virtual ~Grid();

        //void setRows(const Rows & inRows);

        //void setColumns(const Columns & inColumns);

    private:
        friend class Element;
        Grid(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Row;
    class Rows : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Rows>(inParent, inAttr); }

        static const char * Type() { return "rows"; }

        virtual ~Rows();

    private:
        friend class Element;
        Rows(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Column;
    class Columns : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Columns>(inParent, inAttr); }

        static const char * Type() { return "columns"; }

        virtual ~Columns();

        //void addColumn(const Column & inColumn);

    private:
        friend class Element;
        Columns(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Row : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Row>(inParent, inAttr); }

        static const char * Type() { return "row"; }

        virtual ~Row();

    private:
        friend class Element;
        Row(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Column : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Column>(inParent, inAttr); }

        static const char * Type() { return "column"; }

        virtual ~Column();

    private:
        friend class Element;
        Column(Element * inParent, const AttributesMapping & inAttributesMapping);
    };



    class RadioGroup : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<RadioGroup>(inParent, inAttr); }

        static const char * Type() { return "radiogroup"; }

        virtual ~RadioGroup();

    private:
        friend class Element;
        RadioGroup(Element * inParent, const AttributesMapping & inAttributesMapping);
    };



    class Radio : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Radio>(inParent, inAttr); }

        static const char * Type() { return "radio"; }

        virtual ~Radio();

    private:
        friend class Element;
        Radio(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class ProgressMeter : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<ProgressMeter>(inParent, inAttr); }

        static const char * Type() { return "progressmeter"; }

        virtual ~ProgressMeter();

    private:
        friend class Element;
        ProgressMeter(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Deck : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Deck>(inParent, inAttr); }

        static const char * Type() { return "deck"; }

        virtual ~Deck();

    private:
        friend class Element;
        Deck(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Scrollbar : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Scrollbar>(inParent, inAttr); }

        static const char * Type() { return "scrollbar"; }

        virtual ~Scrollbar();

    private:
        friend class Element;
        Scrollbar(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TabBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TabBox>(inParent, inAttr); }

        static const char * Type() { return "tabbox"; }

        virtual ~TabBox();

    private:
        friend class Element;
        TabBox(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Tabs : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Tabs>(inParent, inAttr); }

        static const char * Type() { return "tabs"; }

        virtual ~Tabs();

        //void addRow(const Row & inRow);

    private:
        friend class Element;
        Tabs(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Tab : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Tab>(inParent, inAttr); }

        static const char * Type() { return "tab"; }

        virtual ~Tab();

    private:
        friend class Element;
        Tab(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TabPanel;
    class TabPanels : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TabPanels>(inParent, inAttr); }

        static const char * Type() { return "tabpanels"; }

        virtual ~TabPanels();

    private:
        friend class Element;
        TabPanels(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TabPanel : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TabPanel>(inParent, inAttr); }

        static const char * Type() { return "tabpanel"; }

        virtual ~TabPanel();

    private:
        friend class Element;
        TabPanel(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class GroupBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<GroupBox>(inParent, inAttr); }

        static const char * Type() { return "groupbox"; }

    private:
        friend class Element;
        GroupBox(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Caption : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Caption>(inParent, inAttr); }

        static const char * Type() { return "caption"; }

    private:
        friend class Element;
        Caption(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Tree : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Tree>(inParent, inAttr); }

        virtual ~Tree();

        static const char * Type() { return "tree"; }
    
    private:
        friend class Element;
        Tree(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TreeChildren : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TreeChildren>(inParent, inAttr); }

        static const char * Type() { return "treechildren"; }

        virtual ~TreeChildren();

    private:
        friend class Element;
        TreeChildren(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TreeItem : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TreeItem>(inParent, inAttr); }

        static const char * Type() { return "treeitem"; }

        virtual ~TreeItem();

    private:
        friend class Element;
        TreeItem(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TreeCols : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TreeCols>(inParent, inAttr); }

        static const char * Type() { return "treecols"; }

        virtual ~TreeCols();

    private:
        friend class Element;
        TreeCols(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TreeCol : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TreeCol>(inParent, inAttr); }

        static const char * Type() { return "treecol"; }

        virtual ~TreeCol();

    private:
        friend class Element;
        TreeCol(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TreeRow : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TreeRow>(inParent, inAttr); }

        static const char * Type() { return "treerow"; }

        virtual ~TreeRow();

    private:
        friend class Element;
        TreeRow(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TreeCell : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TreeCell>(inParent, inAttr); }

        static const char * Type() { return "treecell"; }

        virtual ~TreeCell();

    private:
        friend class Element;
        TreeCell(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Statusbar : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Statusbar>(inParent, inAttr); }

        static const char * Type() { return "statusbar"; }

        virtual ~Statusbar();

    private:
        friend class Element;
        Statusbar(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class StatusbarPanel : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<StatusbarPanel>(inParent, inAttr); }

        static const char * Type() { return "statusbarpanel"; }

        virtual ~StatusbarPanel();

    private:
        friend class Element;
        StatusbarPanel(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Toolbar : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Toolbar>(inParent, inAttr); }

        static const char * Type() { return "toolbar"; }

        virtual ~Toolbar();

    private:
        friend class Element;
        Toolbar(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class ToolbarButton : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<ToolbarButton>(inParent, inAttr); }

        static const char * Type() { return "toolbarbutton"; }

        virtual ~ToolbarButton();

    private:
        friend class Element;
        ToolbarButton(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


} // XULWin


#endif // ELEMENT_H_INCLUDED
