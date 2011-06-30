#ifndef GSTMACWIDGET_H_INCLUDED
#define GSTMACWIDGET_H_INCLUDED


extern "C"
{
    #include <gst/gst.h>
    #include <glib.h>
}
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>


namespace Gst
{
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
         * @pipeline: the GStreamer pipeline that you want to connect to your GUI.
         */
        MacWidget(GstElement * pipeline);


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

} // namespace Gst


#endif // GSTMACWIDGET_H_INCLUDED
