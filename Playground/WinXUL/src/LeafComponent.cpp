#include "WinXUL/LeafComponent.h"
#include "WinXUL/ComponentImpl.h"


namespace WinXUL
{

    LeafComponent::LeafComponent(Component * inParentComponent, std::auto_ptr<ComponentImpl> inImpl) :
        ConcreteComponent(inParentComponent, inImpl)
    {
    }


} // namespace WinXUL

