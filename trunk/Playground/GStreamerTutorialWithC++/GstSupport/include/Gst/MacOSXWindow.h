#ifndef XPCOM_VIDEO_MACOSXWINDOW_H_INCLUDED
#define XPCOM_VIDEO_MACOSXWINDOW_H_INCLUDED


#include "NativeWindowImpl.h"
#include <set>


namespace Gst
{

    /**
     * MacOSXWindow
     *
     * Encapsulates a Mac Window.
     * NSView is the WindowId type on Mac.
     */

    class MacOSXWindow : public NativeWindowImpl
    {
    public:
        MacOSXWindow(void * inParentNSView);
        
        virtual ~MacOSXWindow();

        // This class does not create the NSView object.
        // GStreamer does that.
        // Therefore we need this method.
        void setNSView(void * inNSView);

        virtual void move(int x, int y, int w, int h);
        
        virtual void toggleFullScreen();
        
        virtual void setFullScreen(bool inFullscreen);
        
        virtual bool isFullScreen() const;

        virtual void setHidden(bool inHidden);

        virtual void redraw();        
        
        virtual void * parentWindowId();

        virtual void * windowId();

    private:
        void removeNSView();

        void * mParentNSView;
        void * mNSView;
        Logger mLogger;
    };


} // namespace Gst


#endif // XPCOM_VIDEO_MACOSXWINDOW_H_INCLUDED
