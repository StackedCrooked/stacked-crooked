#include "GstSupport/GstWidget.h"
#include "GstSupport/GstSupport.h"
#include <assert.h>
extern "C"
{
    #include <gst/interfaces/xoverlay.h>
}


namespace Gst
{
    
    Widget::Widget(GstElement * inPipeline, WindowId inWindowId) :
        mPipeline(inPipeline),
        mWindowId(inWindowId)
    {
        assert(inPipeline);
        assert(inWindowId);
        
        mOverlay = gst_bin_get_by_interface(GST_BIN(inPipeline), GST_TYPE_X_OVERLAY);
        if (!mOverlay)
        {
            return;
        }

        // Listen to synchronous messages so that we can overlay the xwindow later.
        Gst::ScopedObject<GstBus> bus(gst_pipeline_get_bus(GST_PIPELINE(mPipeline)));
        gst_bus_enable_sync_message_emission(bus.get());
        g_signal_connect(bus.get(), "sync-message", G_CALLBACK(&Widget::OnSyncMessage), this);
    }
    
    
    Widget::~Widget()
    {
        if (mOverlay)
        {
            // No longer listen to synchronous messages.
            Gst::ScopedObject<GstBus> bus(gst_pipeline_get_bus(GST_PIPELINE(mPipeline)));
            gst_bus_disable_sync_message_emission(bus.get());
            g_signal_handlers_disconnect_by_func(bus.get(), (void*)G_CALLBACK(&Widget::OnSyncMessage), this);
        }
    }

    
    void Widget::OnSyncMessage(GstBus * inBus, GstMessage * inMessage, gpointer inData)
    {
        Widget * pThis = static_cast<Widget*>(inData);
        pThis->onSyncMessage(inBus, inMessage);
    }


    void Widget::onSyncMessage(GstBus * inBus, GstMessage * inMessage)
    {
        switch (GST_MESSAGE_TYPE(inMessage))
        {
            case GST_MESSAGE_ELEMENT:
            {   
                if (gst_structure_has_name(inMessage->structure, "prepare-xwindow-id"))
                {
                    gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(mOverlay), (gulong)mWindowId);
                }
                break;
            }
        }
    }

} // namespace Gst
