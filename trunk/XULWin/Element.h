#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED


#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <vector>
#include <windows.h>


namespace XULWin
{

    class ElementFactory;
    class NativeComponent;

    class Element;

    // We use shared_ptr for children.
    typedef boost::shared_ptr<Element> ElementPtr;

    typedef std::vector<ElementPtr> Children;

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

        std::map<std::string, std::string> Attributes;

        boost::shared_ptr<NativeComponent> nativeComponent() const;

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
            explicit ID(const std::string & inID) : mID(inID) {}

            operator const std::string & () const { return mID; }

            bool operator < (const ID & rhs) const { return this->mID < rhs.mID; }
        private:
            std::string mID;
        };

        void add(ElementPtr inChild);

    protected:
        Element(ElementPtr inParent, const Type & inType, const ID & inID, boost::shared_ptr<NativeComponent> inNativeComponent);

        template<class T>
        static ElementPtr Create(ElementPtr inParent, const ID & inID)
        {
            ElementPtr result(new T(inParent, inID));
            inParent->add(result);
            return result;
        }

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
        static ElementPtr Create(ElementPtr inParent, const ID & inID)
        {
            assert(!inParent);
            ElementPtr result(new Window(inID));
            return result;
        }

    private:
        friend class Element;
        Window(const ID & inID);
    };


    class Button : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const ID & inID)
        { return Element::Create<Button>(inParent, inID); }
    
    private:
        friend class Element;
        Button(ElementPtr inParent, const ID & inID);
    };


    class CheckBox : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const ID & inID)
        { return Element::Create<CheckBox>(inParent, inID); }

    private:
        friend class Element;
        CheckBox(ElementPtr inParent, const ID & inID);
    };


    class HBox : public Element
    {
    public:
        static ElementPtr Create(ElementPtr inParent, const ID & inID)
        { return Element::Create<HBox>(inParent, inID); }

    private:
        friend class Element;
        HBox(ElementPtr inParent, const ID & inID);
    };


} // XULWin


#endif // ELEMENT_H_INCLUDED
