#ifndef SVG_H_INCLUDED
#define SVG_H_INCLUDED


#include "Element.h"
#include "AttributeController.h"


namespace XULWin
{

namespace SVG
{

    class SVG : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<SVG>(inParent, inAttr); }

        static const char * Type() { return "svg"; }
    
    private:
        friend class Element;
        SVG(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Group : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Group>(inParent, inAttr); }

        static const char * Type() { return "g"; }
    
    private:
        friend class Element;
        Group(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Polygon : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Polygon>(inParent, inAttr); }

        static const char * Type() { return "polygon"; }
    
    private:
        friend class Element;
        Polygon(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Rect : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Rect>(inParent, inAttr); }

        static const char * Type() { return "rect"; }
    
    private:
        friend class Element;
        Rect(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


    class Path : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Path>(inParent, inAttr); }

        static const char * Type() { return "path"; }
    
    private:
        friend class Element;
        Path(Element * inParent, const AttributesMapping & inAttributesMapping);
    };

} // namespace SVG

} // namespace XULWin

#endif // SVG_H_INCLUDED
