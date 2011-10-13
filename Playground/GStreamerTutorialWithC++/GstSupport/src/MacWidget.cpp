#include "Gst/MacWidget.h"
#include "Gst/Support.h"
#include <assert.h>


namespace Gst {


MacWidget::MacWidget(GstElement * inPipeline, WindowId) :
    mPipeline(inPipeline)
{
    // Listen to synchronous messages
    Gst::ScopedObject<GstBus> bus(gst_pipeline_get_bus(GST_PIPELINE(mPipeline)));
    gst_bus_enable_sync_message_emission(bus.get());
    g_signal_connect(bus.get(), "sync-message::element", G_CALLBACK(&MacWidget::OnSyncMessage), gpointer(this));
}


MacWidget::~MacWidget()
{
    // No longer listen to synchronous messages.
    Gst::ScopedObject<GstBus> bus(gst_pipeline_get_bus(GST_PIPELINE(mPipeline)));
    gst_bus_disable_sync_message_emission(bus.get());
    g_signal_handlers_disconnect_by_func(bus.get(), (void*)G_CALLBACK(&MacWidget::OnSyncMessage), gpointer(this));
}


void MacWidget::setCallback(const Callback & inCallback)
{
    mCallback = inCallback;
}


void MacWidget::setLogger(const Logger & inLogger)
{
    mLogger = inLogger;
}


void MacWidget::OnSyncMessage(GstBus * inBus, GstMessage * inMessage, gpointer inData)
{
    MacWidget * pThis = static_cast<MacWidget*>(inData);
    pThis->onSyncMessage(inBus, inMessage);
}


void MacWidget::onSyncMessage(GstBus * inBus, GstMessage * inMessage)
{
    switch (GST_MESSAGE_TYPE(inMessage))
    {
        case GST_MESSAGE_ELEMENT:
        {   
            if (gst_structure_has_name(inMessage->structure, "have-ns-view"))
            {
                const GValue * value = gst_structure_get_value(inMessage->structure, "nsview");                    
                if (value && G_VALUE_HOLDS_POINTER(value) && mCallback)
                {
                    mCallback(g_value_get_pointer(value));
                }
            }
            break;
        }
    }
}


} // namespace Gst
