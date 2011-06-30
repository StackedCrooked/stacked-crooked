#ifndef WINXUL_WINAPILABEL_H_INCLUDED
#define WINXUL_WINAPILABEL_H_INCLUDED


#include "WinXUL/NativeLabel.h"
#include "WinXUL/WinAPIComponent.h"


namespace WinXUL
{

    class WinAPILabel : public NativeLabel
    {
    public:
        WinAPILabel(HMODULE inModule, NativeComponent * inParent);

        virtual ~WinAPILabel();

    private:
        WinAPIComponent mComponent;
    };


} // namespace WinXUL


#endif // WINXUL_WINAPILABEL_H_INCLUDED
