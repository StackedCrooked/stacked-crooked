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

        const Type & type() const;

        const ID & id() const;

        const Children & children() const { return mChildren; }

        Children & children() { return mChildren; }

        boost::weak_ptr<Element> parent() const { return mParent; }

        AttributesMapping Attributes;

        boost::shared_ptr<NativeComponent> nativeComponent() const;

        void applyAttributes() const;

        class Type
        {
        public:
            explicit Type(const std::string & inType) : mType(inType) {}

            operator const std::string & () const { return mType; }

            bool operator < (const Type & rhs) const { return this->mType < rhs.mType; }
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

        void add(ElementPtr inChild);

    protected:
        Element(const Type & inType, ElementPtr inParent, boost::shared_ptr<NativeComponent> inNativeComponent);

        template<class T>
        static ElementPtr Create(const Type & inType, ElementPtr inParent)
        {
            ElementPtr result(new T(inType, inParent));
            inParent->add(result);
            return result;
        }        

        // this method gets called after construction
        virtual void init();

        boost::weak_ptr<Element> mParent;
        Children mChildren;

    private:
        friend class ElementFactory;
        Type mType;
        ID mID;
        boost::shared_ptr<NativeComponent> mNativeComponent;       
    };


    typedef Element::ID elid;
    typedef Element::Type eltype;

    class Window : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent)
        {
            assert(!inParent);
            ElementPtr result(new Window(inType));
            return result;
        }

        void showModal();

    private:
        friend class Element;
        Window(const Type & inType);
    };


    class Button : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent)
        { return Element::Create<Button>(inType, inParent); }
    
    private:
        friend class Element;
        Button(const Type & inType, ElementPtr inParent);
    };


    class Label : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent)
        { return Element::Create<Label>(inType, inParent); }
    
    private:
        friend class Element;
        Label(const Type & inType, ElementPtr inParent);
    };


    class CheckBox : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent)
        { return Element::Create<CheckBox>(inType, inParent); }

    private:
        friend class Element;
        CheckBox(const Type & inType, ElementPtr inParent);
    };


    class HBox : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent)
        { return Element::Create<HBox>(inType, inParent); }

    private:
        friend class Element;
        HBox(const Type & inType, ElementPtr inParent);
    };


    class VBox : public Element
    {
    public:
        static ElementPtr Create(const Type & inType, ElementPtr inParent)
        { return Element::Create<VBox>(inType, inParent); }

    private:
        friend class Element;
        VBox(const Type & inType, ElementPtr inParent);
    };


} // XULWin


#endif // ELEMENT_H_INCLUDED
