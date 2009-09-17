#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED


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

    /**
     * Represents a XUL element.
     */
    class Element
    {
    public:
        class Type;
        class ID;

        // Override this method to add initialization code
        virtual void init() {}

        const Type & type() const;

        const ID & id() const;

        const Children & children() const { return mChildren; }

        void removeChild(const Element * inChild);

        boost::weak_ptr<Element> parent() const { return mParent; }

        void setAttributes(const AttributesMapping & inAttributes);

        const std::string & getAttribute(const std::string & inName) const;

        void setAttribute(const std::string & inName, const std::string & inValue);

        boost::shared_ptr<NativeComponent> nativeComponent() const;

        class Type
        {
        public:
            explicit Type(const std::string & inType) : mType(inType) {}

            operator const std::string & () const { return mType; }

            bool operator < (const Type & rhs) const { return this->mType < rhs.mType; }

            bool operator == (const Type & rhs) const { return this->mType == rhs.mType; }

        private:
            std::string mType;
        };

        class ID
        {
        public:
            explicit ID(const std::string & inID = "") : mID(inID) {}

            operator const std::string & () const { return mID; }

            bool operator < (const ID & rhs) const { return this->mID < rhs.mID; }
        private:
            std::string mID;
        };

    protected:
        Element(const Type & inType, ElementPtr inParent, boost::shared_ptr<NativeComponent> inNativeComponent);

        template<class T>
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
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

    private:

        // you don't to call this, the factory method takes care of it
        void addChild(ElementPtr inChild);

        friend class ElementFactory;
        Type mType;
        ID mID;
        AttributesMapping mAttributes;
        boost::shared_ptr<NativeComponent> mNativeComponent;
    };


    typedef Element::ID elid;
    typedef Element::Type eltype;

    class Window : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Window>(inType, inParent, inAttr); }

        void showModal();

    private:
        friend class Element;
        Window(const Type & inType, ElementPtr inParent);
    };


    class Button : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Button>(inType, inParent, inAttr); }
    
    private:
        friend class Element;
        Button(const Type & inType, ElementPtr inParent);
    };


    class Label : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Label>(inType, inParent, inAttr); }
    
    private:
        friend class Element;
        Label(const Type & inType, ElementPtr inParent);
    };


    class TextBox : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<TextBox>(inType, inParent, inAttr); }
    
    private:
        friend class Element;
        TextBox(const Type & inType, ElementPtr inParent);
    };


    class CheckBox : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<CheckBox>(inType, inParent, inAttr); }

    private:
        friend class Element;
        CheckBox(const Type & inType, ElementPtr inParent);
    };


    class Box : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<Box>(inType, inParent, inAttr); }

    private:
        friend class Element;
        Box(const Type & inType, ElementPtr inParent);
    };


    class HBox : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<HBox>(inType, inParent, inAttr); }

    private:
        friend class Element;
        HBox(const Type & inType, ElementPtr inParent);
    };


    class VBox : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<VBox>(inType, inParent, inAttr); }

    private:
        friend class Element;
        VBox(const Type & inType, ElementPtr inParent);
    };


    class MenuItem;
    class MenuList : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuList>(inType, inParent, inAttr); }

        void addMenuItem(const MenuItem * inItem);

        void removeMenuItem(const MenuItem * inItem);

    private:
        friend class Element;
        MenuList(const Type & inType, ElementPtr inParent);
    };


    class MenuPopup : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuPopup>(inType, inParent, inAttr); }

        void addMenuItem(const MenuItem * inItem);

        void removeMenuItem(const MenuItem * inItem);

    private:
        friend class Element;
        MenuPopup(const Type & inType, ElementPtr inParent);
    };


    class MenuItem : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent, const AttributesMapping & inAttr)
        { return Element::Create<MenuItem>(inType, inParent, inAttr); }

        virtual ~MenuItem();

        virtual void init();

    private:
        friend class Element;
        MenuItem(const Type & inType, ElementPtr inParent);
    };


} // XULWin


#endif // ELEMENT_H_INCLUDED
