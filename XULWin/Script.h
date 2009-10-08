#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED


#include "Element.h"


namespace XULWin
{

    class Script : public Element
    {
    public:
        static ElementPtr Create(Element * inParent, const AttributesMapping & inAttr)
        { return Element::Create<Script>(inParent, inAttr); }

        static const char * Type() { return "script"; }

        virtual bool init();
    
    private:
        friend class Element;
        Script(Element * inParent, const AttributesMapping & inAttributesMapping);
    };


} // namespace XULWin

#endif // SCRIPT_H_INCLUDED
