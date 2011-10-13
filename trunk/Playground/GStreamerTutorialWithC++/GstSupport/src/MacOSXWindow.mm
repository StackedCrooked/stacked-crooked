#include "Gst/MacOSXWindow.h"
#include "Gst/Logger.h"
#include "Gst/MakeString.h"
#import <AppKit/AppKit.h>
#include <string>



namespace Gst {
    void ToggleVideoPlayerFullScreen();
}


@implementation NSView(ForMacOSXWindow)
- (void)mouseUp:(NSEvent *)theEvent
{
    if([theEvent clickCount] == 2)
    {
        Gst::ToggleVideoPlayerFullScreen();
    }
}
@end


namespace Gst
{

    MacOSXWindow::MacOSXWindow(void * inParentNSView) :
        mParentNSView(inParentNSView),
        mNSView(0)
    {
    }
    
    
    MacOSXWindow::~MacOSXWindow()
    {
        removeNSView();
    }
    
    
    void MacOSXWindow::setNSView(void * inNSView)
    {
        removeNSView();
        mNSView = inNSView;
        [(NSView*)mParentNSView addSubview:(NSView*)mNSView];

        setHidden(true);
    }

    
    void MacOSXWindow::removeNSView()
    {
        if (mNSView)
        {
            setHidden(true);
            [(NSView*)mNSView removeFromSuperviewWithoutNeedingDisplay];
        }
    }


    void MacOSXWindow::move(int x, int y, int w, int h)
    {
        NSView * view = (NSView *)mNSView;
        if (view)
        {
            NSRect rect;
            // Remap to OSX's coordinate system, which is from the bottom left.
            NSView * superview = [view superview];
            rect.origin.x = x;
            rect.origin.y = [superview frame].size.height - y - h;
            rect.size.width = w;
            rect.size.height = h;
            [view setFrame:rect];
        }
    }
        
    
    bool MacOSXWindow::isFullScreen() const
    {
        NSView * view = (NSView *)mNSView;
        return view && [view isInFullScreenMode];
    }
    
    
    void MacOSXWindow::toggleFullScreen()
    {
        setFullScreen(!isFullScreen());
    }
    
    
    void MacOSXWindow::setFullScreen(bool inFullscreen)
    {
        NSView * view = (NSView *)mNSView;
        if (view)
		{
            if (inFullscreen)
            {
                LogInfo("Go full screen");
                [view enterFullScreenMode:[NSScreen mainScreen] withOptions:nil];
            }
            else
            {
                LogInfo("Leave full screen");
                [view exitFullScreenModeWithOptions:nil];
            }
		}
    }


    void MacOSXWindow::setHidden(bool inHidden)
    {
        
        NSView * view = (NSView *)mNSView;
        if (view)
        {
            [view setHidden:(inHidden ? YES : NO)];
        }
    }

    void MacOSXWindow::redraw()
    {
        // No implementation needed.
    }


    void * MacOSXWindow::parentWindowId()
    {
        return mParentNSView;
    }


    void * MacOSXWindow::windowId()
    {
        return mNSView;
    }


} // namespace Gst

