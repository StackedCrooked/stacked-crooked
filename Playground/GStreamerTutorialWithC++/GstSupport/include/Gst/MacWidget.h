#ifndef GSTMACWIDGET_H_INCLUDED
#define GSTMACWIDGET_H_INCLUDED


extern "C"
{
    #include <gst/gst.h>
    #include <glib.h>
}
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>


namespace Gst {


typedef void* WindowId;

/**
 * MacWidget
 *
 * Associates a GStreamer pipeline with a NSView object.
 *
 */
class MacWidget : boost::noncopyable
{
public:

    /**
     * Constructor
     *
     * @param   inPipeline  the GStreamer pipeline that you want to connect to your GUI.
     * @param   inWindowId  ignored, this parameter exist only for conformance with the XWidget constructor.
     */
    MacWidget(GstElement * inPipeline, WindowId inWindowId = 0);


    ~MacWidget();
    

    /**
     * Callback
     *
     * This callback will be invoked (if set) when receiving the "have-ns-view" on MacOSX.
     *
     */
    typedef boost::function<void(WindowId)> Callback;
    
    void setCallback(const Callback & inCallback);
    
    typedef boost::function<void(const std::string &)> Logger;
    void setLogger(const Logger & inLogger);

private:
    static void OnSyncMessage(GstBus * inBus, GstMessage * inMessage, gpointer inData);
    void onSyncMessage(GstBus * inBus, GstMessage * inMessage);
    
    GstElement * mPipeline;
    GstElement * mOverlay;
    Callback mCallback;
    Logger mLogger;
};

typedef MacWidget Widget;
    

} // namespace Gst


#endif // GSTMACWIDGET_H_INCLUDED
