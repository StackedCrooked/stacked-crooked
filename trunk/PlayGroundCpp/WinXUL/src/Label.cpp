#include "WinXUL/Label.h"
#include "WinXUL/ComponentImpl.h"
#include "WinXUL/NativeComponent.h"
#include "WinXUL/NativeLabel.h"
#include <memory>


namespace WinXUL
{
    
    std::auto_ptr<ComponentImpl> CreateNativeLabel(Component * inParent);

    Label::Label(Component * inParent) : 
        LeafComponent(inParent, CreateNativeLabel(inParent))
    {
    }

} // namespace WinXUL

