#ifndef WINXUL_WINAPILABEL_H_INCLUDED
#define WINXUL_WINAPILABEL_H_INCLUDED


#include "WinXUL/NativeLabel.h"
#include "WinXUL/WinAPIComponent.h"


namespace WinXUL
{

    class WinAPILabel : public NativeLabel,
                        public WinAPIComponent
    {
    public:
        WinAPILabel(HMODULE inModule, NativeComponent * inParent);

        virtual ~WinAPILabel();
    };


} // namespace WinXUL


#endif // WINXUL_WINAPILABEL_H_INCLUDED
