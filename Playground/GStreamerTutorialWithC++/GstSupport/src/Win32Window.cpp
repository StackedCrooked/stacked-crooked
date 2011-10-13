#include "Win32Window.h"
#include <sstream>


namespace Gst
{

    Win32Window::Win32Window(HWND inParentHWND) :
        mParentHWND(inParentHWND)
    {
        mMainThreadId = ::GetCurrentThreadId();
        mHWND = ::CreateWindowEx
        (
            0, 
            L"STATIC",
            TEXT(""),
            WS_CHILD,
            0, 0, 0, 0,
            mParentHWND,
            (HMENU)NULL,
            ::GetModuleHandle(NULL),
            0
        );
        ::ShowWindow(mHWND, SW_SHOW);
    }
    

    Win32Window::~Win32Window()
    {
        if (::GetCurrentThreadId() != mMainThreadId)
        {
            logError("Destructor for window not called from main thread id!");
            return;
        }

        ::DestroyWindow(mHWND);
    }
        
        
    void Win32Window::move(int x, int y, int w, int h)
    {
        if (::GetCurrentThreadId() != mMainThreadId)
        {
            logError("Move window not called from main thread id!");
            return;
        }

        //if (x < 0 || x > 10000 || y < 0 || y > 10000 || w < -2 || w > 10000 || h < -2 || h > 10000)
        //{
        //    std::stringstream ss;
        //    ss << "Move window: suspicious values x: " << x << ", y: " << y << ", w: " << w << ", h: " << h << ".";
        //    logError(ss.str());
        //    return;
        //}

        // Use SWP_ASYNCWINDOWPOS to avoid a possible deadlock since
        // we may be calling this from a non-main thread (and the window
        // was created on the main thread).
        ::SetWindowPos(mHWND, NULL, x, y, w, h, SWP_NOZORDER | SWP_ASYNCWINDOWPOS);
    }


    void Win32Window::setHidden(bool inHidden)
    {        
        if (::GetCurrentThreadId() != mMainThreadId)
        {
            logError("Hide window not called from main thread id!");
            return;
        }
        
        ::ShowWindow(mHWND, inHidden ? SW_HIDE : SW_SHOW);
    }
    
    
    void Win32Window::redraw()
    {
        if (::GetCurrentThreadId() != mMainThreadId)
        {
            logError("Redraw window not called from main thread id!");
            return;
        }

        ::RedrawWindow(mParentHWND, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
        ::InvalidateRect(mParentHWND, NULL, TRUE);
    }
		
	
    void * Win32Window::parentWindowId()
    {
        return (void*)mParentHWND;
    }
    
    
    void * Win32Window::windowId()
    {
        return (void*)mHWND;
    }


} // namespace Gst
