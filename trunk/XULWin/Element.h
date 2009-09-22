#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED


#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <map>
#include <string>
#include <vector>


namespace XULWin
{

    class ElementFactory;
    class ElementImpl;

    class Element;

    // We use shared_ptr for children.
    typedef boost::shared_ptr<Element> ElementPtr;

    typedef std::vector<ElementPtr> Children;

    typedef std::map<std::string, std::string> AttributesMapping;

    class Event
    {
    };

    typedef boost::signal<void(Event*)> EventHandler;

    /**
     * Represents a XUL element.
     */
    class Element : private boost::noncopyable
    {
    public:
        ~Element();

        // Override this method to add initialization code
        virtual void init() {}

        const std::string & type() const;

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

        void handleEvent(const std::string & inEvent);

        Element * getElementById(const std::string & inId);

        const Children & children() const { return mChildren; }

        void removeChild(const Element * inChild);

        Element * parent() const { return mParent; }

        std::string getAttribute(const std::string & inName) const;

        void setAttribute(const std::string & inName, const std::string & inValue);

        void setAttributes(const AttributesMapping & inAttributes);

        void initAttributeControllers();

        ElementImpl * getImpl() const;

    protected:
        Element(const std::string & inType, Element * inParent, ElementImpl * inNativeComponent);

        template<class T>
        static ElementPtr Create(Element * inParent,
                                 const AttributesMapping & inAttr)
        {
            ElementPtr result(new T(inParent));
            if (inParent)
            {
                inParent->addChild(result);
            }
            result.get()->initAttributeControllers();
            result->setAttributes(inAttr);
            result->init();
            return result;
        }   

        Element * mParent;
        Children mChildren;
        typedef std::map<std::string, std::vector<EventHandler> > EventHandlers;
        EventHandlers mEventHandlers;

    private:

        // you don't to call this, the factory method takes care of it
        void addChild(ElementPtr inChild);

        friend class ElementFactory;
        std::string mType;
        AttributesMapping mAttributes;
        boost::shared_ptr<ElementImpl> mNativeComponent;
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
        Window(Element * inParent);
    };


    class Button : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Button>(inParent, inAttr); }

        static const char * Type() { return "button"; }
    
    private:
        friend class Element;
        Button(Element * inParent);
    };


    class Label : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Label>(inParent, inAttr); }

        static const char * Type() { return "label"; }

        std::string value() const;
    
    private:
        friend class Element;
        Label(Element * inParent);
    };


    class Description : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Description>(inParent, inAttr); }

        static const char * Type() { return "description"; }
    
    private:
        friend class Element;
        Description(Element * inParent);
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
        Text(Element * inParent);
    };


    class TextBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<TextBox>(inParent, inAttr); }

        static const char * Type() { return "textbox"; }

        EventHandler OnChanged;
    
    private:
        friend class Element;
        TextBox(Element * inParent);
    };


    class CheckBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<CheckBox>(inParent, inAttr); }

        static const char * Type() { return "checkbox"; }

    private:
        friend class Element;
        CheckBox(Element * inParent);
    };


    class Box : public Element
    {
    public: 
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Box>(inParent, inAttr); }

        static const char * Type() { return "box"; }
    private:
        friend class Element;
        Box(Element * inParent);
    };


    class HBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<HBox>(inParent, inAttr); }

        static const char * Type() { return "hbox"; }

    private:
        friend class Element;
        HBox(Element * inParent);
    };


    class VBox : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<VBox>(inParent, inAttr); }

        static const char * Type() { return "vbox"; }

    private:
        friend class Element;
        VBox(Element * inParent);
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
        MenuList(Element * inParent);
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
        MenuPopup(Element * inParent);

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

        virtual void init();

        std::string label() const;

        std::string value() const;

    private:
        friend class Element;
        MenuItem(Element * inParent);
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
        Separator(Element * inParent);
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
        Spacer(Element * inParent);
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
        MenuButton(Element * inParent);
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
        Grid(Element * inParent);
    };


    class Row;
    class Rows : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Rows>(inParent, inAttr); }

        static const char * Type() { return "rows"; }

        virtual ~Rows();

        //void addRow(const Row & inRow);

    private:
        friend class Element;
        Rows(Element * inParent);
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
        Columns(Element * inParent);
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
        Row(Element * inParent);
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
        Column(Element * inParent);
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
        RadioGroup(Element * inParent);
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
        Radio(Element * inParent);
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
        ProgressMeter(Element * inParent);
    };


} // XULWin


#endif // ELEMENT_H_INCLUDED
