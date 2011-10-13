#ifndef GST_MANUALBUSLISTENER_H_INCLUDED
#define GST_MANUALBUSLISTENER_H_INCLUDED


#include "Gst/Support.h"
extern "C"
{
    #include <gst/gst.h>
    #include <glib.h>
}
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/signal.hpp>
#include <set>


namespace Gst
{

    /**
     * ManualBusListener handles incoming messages.
     * The program has to call 'checkForMessages' periodically to receive any new messages.
     * This class can be used as an alternative to the BusListener class in situations where a
     * GMainLoop can not be used.
     */
    class ManualBusListener : boost::noncopyable
    {
    public:
        ManualBusListener(GstElement * inPipeline);        
        
        typedef boost::function<void(GstMessage * )> Handler;
        typedef std::vector<Handler> Handlers;
                
        void addHandler(GstMessageType inMessageType, const Handler & inHandler);
        
        // Callback for all messages that have no dedicated handler.
        void addFallbackHandler(const Handler & inHandler);
        
        // Call this method perically to process images.
        void checkForMessages();

    private:
        ScopedObject<GstElement> mPipeline;
        ScopedObject<GstBus> mBus;
        typedef std::map<GstMessageType, Handlers> HandlersMapping;
        HandlersMapping mHandlers;
        Handler mFallbackHandler;
    };

} // namespace Gst


#endif // GST_MANUALBUSLISTENER_H_INCLUDED
