#ifndef WINXUL_NATIVELABEL_H_INCLUDED
#define WINXUL_NATIVELABEL_H_INCLUDED


#include "WinXUL/NativeComponent.h"


namespace WinXUL
{

    /**
     * NativeLabel is the base class native label classes (WinAPILabel, IUPLabel, ...)
     */
    class NativeLabel : public NativeComponent
    {
    public:
        NativeLabel(NativeComponent * inNativeParent);

        virtual ~NativeLabel();
    };


} // namespace WinXUL


#endif // WINXUL_NATIVELABEL_H_INCLUDED
