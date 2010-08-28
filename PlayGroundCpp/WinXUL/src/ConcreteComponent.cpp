#include "WinXUL/ConcreteComponent.h"
#include "WinXUL/ComponentImpl.h"


namespace WinXUL
{

    ConcreteComponent::ConcreteComponent(Component * inParent, std::auto_ptr<ComponentImpl> inImpl) :
        Component(),
        mParent(inParent),
        mImpl(inImpl.release()),
        mElement(0)
    {
    }


    ConcreteComponent::~ConcreteComponent()
    {
    }

    
    Component * ConcreteComponent::getParent()
    {
        return mParent;
    }


    
    const Component * ConcreteComponent::getParent() const
    {
        return mParent;
    }

    
    int ConcreteComponent::getWidth() const
    {
        return 0;
    }


    int ConcreteComponent::getHeight() const
    {
        return 0;
    }

} // namespace WinXUL
