#include "WinXUL/Decorator.h"


namespace WinXUL
{

    Decorator::Decorator(std::auto_ptr<Component> inDecoratedComponent) :
        mDecoratedComponent(inDecoratedComponent)
    {
    }


    Decorator::~Decorator()
    {

    }


    Component * Decorator::getDecoratedComponent()
    {
        return mDecoratedComponent.get();
    }


    const Component * Decorator::getDecoratedComponent() const
    {
        return mDecoratedComponent.get();
    }

    
    int Decorator::getWidth() const
    {
        return mDecoratedComponent->getWidth();
    }

    
    int Decorator::getHeight() const
    {
        return mDecoratedComponent->getHeight();
    }

} // namespace WinXUL
