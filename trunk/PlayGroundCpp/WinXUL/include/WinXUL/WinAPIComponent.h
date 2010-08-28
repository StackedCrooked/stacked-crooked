#ifndef WINXUL_WINAPICOMPONENT_H_INCLUDED
#define WINXUL_WINAPICOMPONENT_H_INCLUDED


#include <windows.h>


namespace WinXUL
{

    class WinAPIComponent
    {
    public:
        WinAPIComponent(HMODULE inModule,
                        HWND inParent,
                        LPCTSTR inClassName,
                        DWORD inExStyle,
                        DWORD inStyle);

        virtual ~WinAPIComponent();

        HWND handle();

        const HWND handle() const;

        HMODULE module();

        const HMODULE module() const;

    private:
        WinAPIComponent(const WinAPIComponent &);
        WinAPIComponent & operator=(const WinAPIComponent &);

        HMODULE mModule;
        HWND mParent;
        HWND mHWND;
        int mId;
        static int sId;
    };


} // namespace WinXUL


#endif // WINXUL_WINAPICOMPONENT_H_INCLUDED
