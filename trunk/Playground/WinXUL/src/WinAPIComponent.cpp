#include "WinXUL/WinAPIComponent.h"
#include <stdexcept>


namespace WinXUL
{

    // This is the first id value used for identifying components.
    int WinAPIComponent::sId(101);

    WinAPIComponent::WinAPIComponent(HMODULE inModule,
                                     HWND inParent,
                                     LPCTSTR inClassName,
                                     DWORD inExStyle,
                                     DWORD inStyle) :
        mModule(inModule),
        mParent(inParent),
        mHWND(0),
        mId(sId++)
    {
        mHWND = ::CreateWindowEx(inExStyle,
                                   inClassName,
                                   TEXT(""),
                                   inStyle | WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
                                   0, 0, 0, 0,
                                   inParent,
                                   (HMENU)mId,
                                   mModule,
                                   0);

        if (!mHWND)
        {
            throw std::runtime_error("Failed to create window.");
        }

        // Use the modern font.
        NONCLIENTMETRICS ncm;
        ncm.cbSize = sizeof(NONCLIENTMETRICS);
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
        HFONT hFont = ::CreateFontIndirect(&ncm.lfMessageFont);
        ::SendMessage(mHWND, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
    }


    WinAPIComponent::~WinAPIComponent()
    {
        if (mHWND)
        {
            ::DestroyWindow(mHWND);
        }
    }


    HWND WinAPIComponent::handle()
    {
        return mHWND;
    }


    const HWND WinAPIComponent::handle() const
    {
        return mHWND;
    }


    HMODULE WinAPIComponent::module()
    {
        return mModule;
    }


    const HMODULE WinAPIComponent::module() const
    {
        return mModule;
    }

} // namespace WinXUL
