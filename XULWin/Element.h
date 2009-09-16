#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED


#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <set>
#include <string>
#include <windows.h>


namespace XULWin
{

    class ElementFactory;
    class NativeComponent;

    class Element;

    // We use shared_ptr for children.
    typedef boost::shared_ptr<Element> ElementPtr;

    // Using weak_ptr for reference from child to parent.
    typedef boost::weak_ptr<Element> ElementWPtr;

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

    protected:
        Element(ElementPtr inParent, const Type & inType, const ID & inID, boost::shared_ptr<NativeComponent> inNativeComponent);

        void add(ElementPtr inChild);

    private:

        friend class ElementFactory;
        ElementWPtr mParent;
        Type mType;
        ID mID;
        boost::shared_ptr<NativeComponent> mNativeComponent;
        std::set<ElementPtr> mChildren;

        
    };


    class Window : public Element
    {
    public:
        Window(const ID & inID);
    };


    class Button : public Element
    {
    public:
        Button(ElementPtr inParent, const ID & inID);
    };


} // XULWin


#endif // ELEMENT_H_INCLUDED
