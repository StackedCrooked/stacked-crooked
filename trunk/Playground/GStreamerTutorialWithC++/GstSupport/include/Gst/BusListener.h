#ifndef GSTBUSLISTENER_H_INCLUDED
#define GSTBUSLISTENER_H_INCLUDED


#include "Gst/Support.h"
extern "C"
{
    #include <gst/gst.h>
    #include <glib.h>
}
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <set>


namespace Gst
{

    /**
     * BusListener
     *
     * Inherit this class if you want to receive bus messages.
     * Use ScopedBusListener if you prefer callbacks over inheritance.
     *
     * NOTE:
     * There has to be a running GMainLoop in order to receive bus messages.
     * If you don't want to set up a GMainLoop instance then you can also
     * get the bus messages using gst_bus_have_pending, gst_bus_peek and 
     * gst_bus_pop function. However, in that scenario you won't be needing
     * this class.
     *
     */
    class BusListener : boost::noncopyable
    {
    public:
        BusListener(GstElement * inPipeline);

        virtual ~BusListener();
        
    protected:
        /**
         * onBusMessage
         *
         * Override this method to receive the bus messages.
         *
         * This method should return by calling the parent:
         *     return BusListener::onBusMessage(inBus, inMessage);
         */
        virtual gboolean onBusMessage(GstBus * inBus, GstMessage * inMessage);

    private:
        static gboolean OnBusMessage(GstBus * inBus, GstMessage * inMessage, gpointer inData);

        ScopedObject<GstBus> mBus;

        typedef std::set<BusListener*> Instances;
        static Instances sInstances;
    };


    /**
     * ScopedBusListener
     *
     * Listen to bus messages through a callback.
     */
    class ScopedBusListener : public BusListener
    {
    public:
        // The callback implementation should normally return TRUE.
        // If can unregister itself as listener by returning FALSE.
        typedef boost::function<gboolean(GstBus *, GstMessage*)> Callback;

        ScopedBusListener(GstElement * inPipeline, const Callback & inCallback);

    protected:
        virtual gboolean onBusMessage(GstBus * inBus, GstMessage * inMessage);

        Callback mCallback;
    };

} // namespace Gst


#endif // GSTBUSLISTENER_H_INCLUDED
