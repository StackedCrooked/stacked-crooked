#include "WinXUL/NativeComponent.h"
#include "WinXUL/Decorator.h"


namespace WinXUL
{

    NativeComponent::NativeComponent(NativeComponent * inNativeParent) :
        ComponentImpl(),
        mNativeParent(inNativeParent),
        mWindowId(0),
        mModuleId(0)
    {
    }


    NativeComponent::~NativeComponent()
    {
    }


    NativeComponent * NativeComponent::FindNativeParent(Component * inComponent)
    {
        if (inComponent)
        {
            if (Decorator * decorator = dynamic_cast<Decorator*>(inComponent))
            {
                return FindNativeParent(decorator->getDecoratedComponent());
            }
            else if (NativeComponent * nativeComponent = dynamic_cast<NativeComponent*>(inComponent))
            {
                return nativeComponent;
            }
            else
            {
                return FindNativeParent(inComponent->getParent());
            }
        }
        return 0;
    }


    void * NativeComponent::getWindowId()
    {
        return mWindowId;
    }


    const void * NativeComponent::getWindowId() const
    {
        return mWindowId;
    }


    void * NativeComponent::getModuleId()
    {
        return mModuleId;
    }


    const void * NativeComponent::getModuleId() const
    {
        return mModuleId;
    }


    void NativeComponent::setWindowId(void * inWindowId)
    {
        mWindowId = inWindowId;
    }


    void NativeComponent::setModuleId(void * inModuleId)
    {
        mModuleId = inModuleId;
    }

} // namespace WinXUL
