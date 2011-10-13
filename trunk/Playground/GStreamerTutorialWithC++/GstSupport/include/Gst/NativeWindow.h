#ifndef XPCOM_VIDEO_NATIVEWINDOW_H_INCLUDED
#define XPCOM_VIDEO_NATIVEWINDOW_H_INCLUDED


#include "Fallible.h"
#include <boost/scoped_ptr.hpp>


namespace Gst
{

    class NativeWindowImpl;

    class NativeWindow
    {
    public:
        NativeWindow(void * inParentWnd);

        // Will maintain a cache of the current position.
        // If the parameters are equal to cached values then no move is done.
        // Returns true only if a real move was done.
        void move(int x, int y, int w, int h);

        void setHidden(bool inHidden);  
        
        bool isFullScreen();
        
        void toggleFullScreen();
    
        void setFullScreen(bool inFullscreen);

        void redraw();
        
        void * parentWindowId();

        void * windowId();

        // It is not nice to expose implementation deatails.
        // However, on Mac we need to access the setNSView method.
        NativeWindowImpl * impl() { return mImpl.get(); }

    private:
        boost::scoped_ptr<NativeWindowImpl> mImpl;
        Fallible<int> mCachedX, mCachedY, mCachedWidth, mCachedHeight;
        Fallible<bool> mCachedHidden;
    };


} // namespace Gst


#endif // XPCOM_VIDEO_NATIVEWINDOW_H_INCLUDED
