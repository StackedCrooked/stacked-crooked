#include "Window.h"
#include <string>


namespace XULWin
{
    int CommandID::sID = 101; // start command IDs at 101 to avoid conflicts with Windows predefined values

    NativeComponent::NativeComponent(NativeComponentPtr inParent, CommandID inCommandID, LPCWSTR inClassName, DWORD inExStyle, DWORD inStyle) :
        mParent(inParent),
        mModuleHandle(::GetModuleHandle(0)), // TODO: Fix this hacky thingy!
        mCommandID(inCommandID)
    {
        NativeComponentPtr parent = mParent.lock();
        if (!parent && (inStyle & WS_CHILD))
        {
            throw std::runtime_error("Invalid parent");
        }

        int x = 100;
        int y = 100;
        int w = 200;
        int h = 300;
        if (parent)
        {
            RECT rc;
            ::GetClientRect(parent->handle(), &rc);
            x = 0;
            y = 0;
            w = rc.right - rc.left;
            h = rc.bottom - rc.top;
        }
        else
        {
            x = (GetSystemMetrics(SM_CXSCREEN) - w)/2;
            y = (GetSystemMetrics(SM_CYSCREEN) - h)/2;
        }

        
        mHandle = ::CreateWindowEx
        (
            inExStyle, 
            inClassName,
            L"",            // title attribute can be set later
            inStyle,
            x, y, w, h,
            parent ? parent->handle() : 0,
            (HMENU)inCommandID.intValue(),
            mModuleHandle,
            0
        );
    }


    NativeComponentPtr NativeComponent::parent() const
    {
        NativeComponentPtr result;
        if (NativeComponentPtr parent = mParent.lock())
        {
            result = parent;
        }
        return result;
    }


    HWND NativeComponent::handle() const
    {
        return mHandle;
    }


    void NativeWindow::Register(HMODULE inModuleHandle, WNDPROC inWndProc)
    {
        WNDCLASSEX wndClass;
        wndClass.cbSize = sizeof(wndClass);
        wndClass.style = 0;
        wndClass.lpfnWndProc = inWndProc;
        wndClass.cbClsExtra    = 0;
        wndClass.cbWndExtra    = 0;
        wndClass.hInstance    = inModuleHandle;
        wndClass.hIcon = 0;
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = 0; // covered by content pane so no color needed (reduces flicker)
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = TEXT("XULWin::Window");
        wndClass.hIconSm = 0;
        if (! RegisterClassEx(&wndClass))
        {
            throw std::runtime_error(std::string("Could not register Windows class."));
        }
    }

} // namespace XULWin
