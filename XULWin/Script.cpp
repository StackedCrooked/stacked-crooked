#include "Script.h"
#include "AttributeController.h"
#include "Decorator.h"


namespace XULWin
{

    class NativeScript : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeScript(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        bool initAttributeControllers();
    };


    Script::Script(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Script::Type(),
                inParent,
                new Decorator(new NativeScript(inParent->impl(), inAttributesMapping)))
    {
    }


    bool Script::init()
    {
        setAttribute("value", innerText());
        return Element::init();
    }

    
    NativeScript::NativeScript(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualComponent(inParent, inAttributesMapping)
    {
    }


    int NativeScript::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return 0;
    }

    
    int NativeScript::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return 0;
    }


    bool NativeScript::initAttributeControllers()
    {
        return Super::initAttributeControllers();
    }


} // namespace XULWin
