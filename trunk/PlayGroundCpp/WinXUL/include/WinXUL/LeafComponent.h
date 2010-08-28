#ifndef WINXUL_LEAFCOMPONENT_H_INCLUDED
#define WINXUL_LEAFCOMPONENT_H_INCLUDED


#include "WinXUL/ConcreteComponent.h"
#include <memory>


namespace WinXUL
{

    /**
     * LeafComponent the base class for non-container components.
     */
    class LeafComponent : public ConcreteComponent
    {
    protected:
        LeafComponent(Component * inParentComponent, std::auto_ptr<ComponentImpl> inImpl);
    };

} // namespace WinXUL


#endif // WINXUL_LEAFCOMPONENT_H_INCLUDED
