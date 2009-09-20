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
    class NativeComponent;

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

        EventHandler OnCommand;

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

        std::string label() const;

        void setLabel(const std::string & inLabel);

        void handleEvent(const std::string & inEvent);

        ElementPtr getElementById(const std::string & inId);

        const Children & children() const { return mChildren; }

        void removeChild(const Element * inChild);

        Element * parent() const { return mParent; }

        std::string getAttribute(const std::string & inName) const;

        void setAttribute(const std::string & inName, const std::string & inValue);

        void setAttributes(const AttributesMapping & inAttributes);

        boost::shared_ptr<NativeComponent> nativeComponent() const;

    protected:
        Element(const std::string & inType, ElementPtr inParent, boost::shared_ptr<NativeComponent> inNativeComponent);

        template<class T>
        static ElementPtr Create(ElementPtr inParent,
                                 const AttributesMapping & inAttr)
        {
            ElementPtr result(new T(inParent));
            if (inParent)
            {
                inParent->addChild(result);
            }
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
        boost::shared_ptr<NativeComponent> mNativeComponent;
    };

    class Window : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Window>(inParent, inAttr); }

        static const char * Type() { return "window"; }

        void showModal();

    private:
        friend class Element;
        Window(ElementPtr inParent);
    };


    class Button : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Button>(inParent, inAttr); }

        static const char * Type() { return "button"; }
    
    private:
        friend class Element;
        Button(ElementPtr inParent);
    };


    class Label : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Label>(inParent, inAttr); }

        static const char * Type() { return "label"; }

        std::string value() const;
    
    private:
        friend class Element;
        Label(ElementPtr inParent);
    };


    // Text is the same as label
    class Text : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Text>(inParent, inAttr); }

        static const char * Type() { return "text"; }
    
    private:
        friend class Element;
        Text(ElementPtr inParent);
    };


    class TextBox : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<TextBox>(inParent, inAttr); }

        static const char * Type() { return "textbox"; }

        EventHandler OnChanged;
    
    private:
        friend class Element;
        TextBox(ElementPtr inParent);
    };


    class CheckBox : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<CheckBox>(inParent, inAttr); }

        static const char * Type() { return "checkbox"; }

    private:
        friend class Element;
        CheckBox(ElementPtr inParent);
    };


    class Box : public Element
    {
    public: 
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Box>(inParent, inAttr); }

        static const char * Type() { return "box"; }
    private:
        friend class Element;
        Box(ElementPtr inParent);
    };


    class HBox : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<HBox>(inParent, inAttr); }

        static const char * Type() { return "hbox"; }

    private:
        friend class Element;
        HBox(ElementPtr inParent);
    };


    class VBox : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<VBox>(inParent, inAttr); }

        static const char * Type() { return "vbox"; }

    private:
        friend class Element;
        VBox(ElementPtr inParent);
    };


    class MenuItem;
    class MenuList : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuList>(inParent, inAttr); }

        static const char * Type() { return "menulist"; }

        void addMenuItem(const MenuItem * inItem);

        void removeMenuItem(const MenuItem * inItem);

    private:
        friend class Element;
        MenuList(ElementPtr inParent);
    };


    class MenuPopup : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuPopup>(inParent, inAttr); }

        virtual ~MenuPopup();

        static const char * Type() { return "menupopup"; }

        void addMenuItem(const MenuItem * inItem);

        void removeMenuItem(const MenuItem * inItem);

    private:
        friend class Element;
        MenuPopup(ElementPtr inParent);

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
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuItem>(inParent, inAttr); }

        static const char * Type() { return "menuitem"; }

        virtual ~MenuItem();

        virtual void init();

        std::string label() const;

        std::string value() const;

    private:
        friend class Element;
        MenuItem(ElementPtr inParent);
    };


    class Separator : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Separator>(inParent, inAttr); }

        static const char * Type() { return "separator"; }

        virtual ~Separator();

    private:
        friend class Element;
        Separator(ElementPtr inParent);
    };


    class MenuButton : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuButton>(inParent, inAttr); }

        static const char * Type() { return "menubutton"; }

        virtual ~MenuButton();

    private:
        friend class Element;
        MenuButton(ElementPtr inParent);
    };


    class Rows;
    class Columns;
    class Grid : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Grid>(inParent, inAttr); }

        static const char * Type() { return "grid"; }

        virtual ~Grid();

        void setRows(const Rows & inRows);

        void setColumns(const Columns & inColumns);

    private:
        friend class Element;
        Grid(ElementPtr inParent);
    };


    class Row;
    class Rows : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Rows>(inParent, inAttr); }

        static const char * Type() { return "rows"; }

        virtual ~Rows();

        void addRow(const Row & inRow);

    private:
        friend class Element;
        Rows(ElementPtr inParent);
    };


    class Column;
    class Columns : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Columns>(inParent, inAttr); }

        static const char * Type() { return "columns"; }

        virtual ~Columns();

        void addColumn(const Column & inColumn);

    private:
        friend class Element;
        Columns(ElementPtr inParent);
    };


    class Row : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Row>(inParent, inAttr); }

        static const char * Type() { return "row"; }

        virtual ~Row();

        virtual void init();

    private:
        friend class Element;
        Row(ElementPtr inParent);
    };


    class Column : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Column>(inParent, inAttr); }

        static const char * Type() { return "column"; }

        virtual ~Column();

        virtual void init();

        void addColumn(const Row & inRow);

    private:
        friend class Element;
        Column(ElementPtr inParent);
    };


} // XULWin


#endif // ELEMENT_H_INCLUDED
