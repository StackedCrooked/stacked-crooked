#include "WinXUL/WinAPILabel.h"


namespace WinXUL
{

    WinAPILabel::WinAPILabel(HMODULE inModule, NativeComponent * inNativeParent) :
        NativeLabel(inNativeParent),
        WinAPIComponent(inModule, static_cast<HWND>(inNativeParent->getWindowId()), TEXT("STATIC"), 0, 0)
    {
        NativeComponent::setModuleId(WinAPIComponent::module());
        NativeComponent::setWindowId(WinAPIComponent::handle());
    }


    WinAPILabel::~WinAPILabel()
    {
    }

} // namespace WinXUL
