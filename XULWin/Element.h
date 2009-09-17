#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED


#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
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
    public:
        
    };

    typedef boost::function<void(Event*)> EventHandler;

    /**
     * Represents a XUL element.
     */
    class Element
    {
    public:

        // Override this method to add initialization code
        virtual void init() {}

        const std::string & type() const;

        void addEventListener(const std::string & inEvent, const EventHandler & inEventHandler);

        ElementPtr getElementById(const std::string & inID);

        const Children & children() const { return mChildren; }

        void removeChild(const Element * inChild);

        boost::weak_ptr<Element> parent() const { return mParent; }

        void setAttributes(const AttributesMapping & inAttributes);

        const std::string & getAttribute(const std::string & inName) const;

        void setAttribute(const std::string & inName, const std::string & inValue);

        boost::shared_ptr<NativeComponent> nativeComponent() const;

    protected:
        Element(const std::string & inType, ElementPtr inParent, boost::shared_ptr<NativeComponent> inNativeComponent);

        template<class T>
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        {
            ElementPtr result(new T(inType, inParent));
            if (inParent)
            {
                inParent->addChild(result);
            }
            result->setAttributes(inAttr);
            result->init();
            return result;
        }   

        boost::weak_ptr<Element> mParent;
        Children mChildren;
        std::map<std::string, std::vector<EventHandler> > mEventHandlers;

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
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Window>(inType, inParent, inAttr); }

        void showModal();

    private:
        friend class Element;
        Window(const std::string & inType, ElementPtr inParent);
    };


    class Button : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Button>(inType, inParent, inAttr); }
    
    private:
        friend class Element;
        Button(const std::string & inType, ElementPtr inParent);
    };


    class Label : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Label>(inType, inParent, inAttr); }
    
    private:
        friend class Element;
        Label(const std::string & inType, ElementPtr inParent);
    };


    class TextBox : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<TextBox>(inType, inParent, inAttr); }
    
    private:
        friend class Element;
        TextBox(const std::string & inType, ElementPtr inParent);
    };


    class CheckBox : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<CheckBox>(inType, inParent, inAttr); }

    private:
        friend class Element;
        CheckBox(const std::string & inType, ElementPtr inParent);
    };


    class Box : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Box>(inType, inParent, inAttr); }

    private:
        friend class Element;
        Box(const std::string & inType, ElementPtr inParent);
    };


    class HBox : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<HBox>(inType, inParent, inAttr); }

    private:
        friend class Element;
        HBox(const std::string & inType, ElementPtr inParent);
    };


    class VBox : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<VBox>(inType, inParent, inAttr); }

    private:
        friend class Element;
        VBox(const std::string & inType, ElementPtr inParent);
    };


    class MenuItem;
    class MenuList : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuList>(inType, inParent, inAttr); }

        void addMenuItem(const MenuItem * inItem);

        void removeMenuItem(const MenuItem * inItem);

    private:
        friend class Element;
        MenuList(const std::string & inType, ElementPtr inParent);
    };


    class MenuPopup : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuPopup>(inType, inParent, inAttr); }

        void addMenuItem(const MenuItem * inItem);

        void removeMenuItem(const MenuItem * inItem);

    private:
        friend class Element;
        MenuPopup(const std::string & inType, ElementPtr inParent);
    };


    class MenuItem : public Element
    {
    public:
        static ElementPtr Create(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuItem>(inType, inParent, inAttr); }

        virtual ~MenuItem();

        virtual void init();

    private:
        friend class Element;
        MenuItem(const std::string & inType, ElementPtr inParent);
    };


} // XULWin


#endif // ELEMENT_H_INCLUDED
