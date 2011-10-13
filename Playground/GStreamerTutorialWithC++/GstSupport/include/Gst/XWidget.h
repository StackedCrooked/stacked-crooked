#ifndef GST_XWIDGET_H_INCLUDED
#define GST_XWIDGET_H_INCLUDED


#include <boost/noncopyable.hpp>
extern "C"
{
    #include <gst/gst.h>
    #include <glib.h>
}


namespace Gst {


// Pointer type for platform-dependent Window handle.
typedef void* WindowId;

/**
 * Widget
 *
 * Associates a GStreamer pipeline with a window handle.
 * It does nothing more than just that.
 *
 * This can be used for Windows, Gtk, Qt.
 *
 * Note: this class does not work for Mac! See GstMacWidget for that.
 */
class XWidget : boost::noncopyable
{
public:

    /**
     * Constructor
     *
     * @pipeline: the GStreamer pipeline that you want to display on a WindowId.
     * @inWindowId: the window handle that you want the video to be displayed on.
     */
    XWidget(GstElement * pipeline, WindowId inWindowId);

    ~XWidget();

private:
    static void OnSyncMessage(GstBus * inBus, GstMessage * inMessage, gpointer inData);
    void onSyncMessage(GstBus * inBus, GstMessage * inMessage);
    
    GstElement * mPipeline;
    GstElement * mOverlay;
    WindowId mWindowId;
};


typedef XWidget Widget;


} // namespace Gst


#endif // GST_XWIDGET_H_INCLUDED
