#include "Gst/NativeWindow.h"
#include "Gst/NativeWindowImpl.h"


#if defined(__APPLE__)
#include "Gst/MacOSXWindow.h"
#elif defined(_WIN32)
#include "Gst/Win32Window.h"
#elif defined(linux)
#include "Gst/GdkWindow.h"
#else
#error Unsupported Platform
#endif


namespace Gst
{

    NativeWindowImpl * CreateImpl(void * inParentWnd)
    {
        #if defined(_WIN32)
        return new Win32Window((HWND)inParentWnd);
        #elif defined(__APPLE__)
        return new MacOSXWindow(inParentWnd);
        #elif defined(linux)
        return new GdkWindow(inParentWnd);
        #endif
    }

    NativeWindow::NativeWindow(void * inParentWnd) :
        mImpl(CreateImpl(inParentWnd))
    {
    }
        
            
    void NativeWindow::move(int x, int y, int w, int h)
    {
        if (mCachedX.isValid() && mCachedX.getValue() == x && 
            mCachedY.isValid() && mCachedY.getValue() == y && 
            mCachedWidth.isValid() && mCachedWidth.getValue() == w && 
            mCachedHeight.isValid() && mCachedHeight.getValue() == h)
        {
            return;
        }

        assert(mImpl);
        if (mImpl)
        {
            mImpl->move(x, y, w, h);
            mCachedX = x;
            mCachedY = y;
            mCachedWidth = w;
            mCachedHeight = h;
        }
        return;
    }
    
    
    bool NativeWindow::isFullScreen()
    {
        if (mImpl)
        {
            return mImpl->isFullScreen();
        }
        return false;        
    }
    
    
    void NativeWindow::toggleFullScreen()
    {
        if (mImpl)
        {
            mImpl->toggleFullScreen();
        }
    }
    
    
    void NativeWindow::setFullScreen(bool inFullscreen)
    {
        if (mImpl)
        {
            mImpl->setFullScreen(inFullscreen);
        }
    }


    void NativeWindow::setHidden(bool inHidden)
    {
        if (mCachedHidden.isValid() && mCachedHidden.getValue() == inHidden)
        {
            return;
        }

        assert(mImpl);
        if (mImpl)
        {
            mImpl->setHidden(inHidden);
            mCachedHidden = inHidden;
        }
    }
        
            
    void NativeWindow::redraw()
    {
        assert(mImpl);
        if (mImpl)
        {
            mImpl->redraw();
        }
    }
    

    void * NativeWindow::parentWindowId()
    {
        assert(mImpl);
        if (mImpl)
        {
            return mImpl->parentWindowId();
        }
        return NULL;
    }


    void * NativeWindow::windowId()
    {
        assert(mImpl);
        if (mImpl)
        {
            return mImpl->windowId();
        }
        return NULL;
    }


} // namespace Gst
