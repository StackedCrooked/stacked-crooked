#include "WinXUL/NativeLabel.h"
#include <memory>


namespace WinXUL
{

    NativeLabel::NativeLabel(NativeComponent * inNativeParent) :
        NativeComponent(inNativeParent)
    {
    }


    NativeLabel::~NativeLabel()
    {
    }

} // namespace WinXUL



#ifdef WIN32
#include "WinXUL/WinAPILabel.h"
namespace WinXUL
{
    std::auto_ptr<ComponentImpl> CreateNativeLabel(Component * inParent)
    {   
        std::auto_ptr<ComponentImpl> result;
        if (NativeComponent * parent = NativeComponent::FindNativeParent(inParent))
        {
            result.reset(new WinAPILabel(static_cast<HMODULE>(parent->getModuleId()), parent));
        }
        return result;
    }
} // namespace WinXUL
#else
#pragma error("Unsupported platform.")
#endif
