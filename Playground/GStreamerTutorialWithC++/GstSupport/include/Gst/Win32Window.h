#ifndef XPCOM_VIDEO_WIN32WINDOW_H_INCLUDED
#define XPCOM_VIDEO_WIN32WINDOW_H_INCLUDED


#include "NativeWindowImpl.h"
#include <windows.h>


namespace Gst
{

    class Win32Window : public NativeWindowImpl
    {
    public:
        Win32Window(HWND inParentHWND);

        virtual ~Win32Window();

        virtual void move(int x, int y, int w, int h);

        virtual void setHidden(bool inHidden);

        virtual void redraw();
        
        virtual void * parentWindowId();

        virtual void * windowId();

    private:
        HWND mParentHWND;
        HWND mHWND;
        DWORD mMainThreadId;
    };


} // namespace Gst


#endif // XPCOM_VIDEO_WIN32WINDOW_H_INCLUDED
