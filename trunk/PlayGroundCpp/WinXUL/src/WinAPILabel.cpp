#include "WinXUL/WinAPILabel.h"


namespace WinXUL
{

    WinAPILabel::WinAPILabel(HMODULE inModule, NativeComponent * inNativeParent) :
        NativeLabel(inNativeParent),
        mComponent(inModule,
                   static_cast<HWND>(inNativeParent->getWindowId()),
                   TEXT("STATIC"),
                   0,
                   0)
    {
        NativeComponent::setModuleId(mComponent.module());
        NativeComponent::setWindowId(mComponent.handle());
    }


    WinAPILabel::~WinAPILabel()
    {
    }

} // namespace WinXUL
