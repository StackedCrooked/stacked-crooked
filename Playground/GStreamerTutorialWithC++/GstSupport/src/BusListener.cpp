#include "Gst/BusListener.h"


namespace Gst
{

    BusListener::Instances BusListener::sInstances;


    BusListener::BusListener(GstElement * inPipeline)
    {
        mBus.reset(gst_pipeline_get_bus(GST_PIPELINE(inPipeline)));
        gst_bus_add_watch(mBus.get(), &BusListener::OnBusMessage, this);
        sInstances.insert(this);
    }


    BusListener::~BusListener()
    {
        sInstances.erase(this);
    }
    
    
    gboolean BusListener::OnBusMessage(GstBus * inBus, GstMessage * inMessage, gpointer inData)
    {
        // What we do is:
        // - Check if the instance still exists.
        // - If it is found then we call onBusMessage on it.
        // - If it is not found then return FALSE, which removes the bus watch.
        //
        BusListener * pThis = static_cast<BusListener*>(inData);
        
        Instances::iterator it = sInstances.find(pThis);
        if (it != sInstances.end())
        {
            return pThis->onBusMessage(inBus, inMessage);
        }

        // Returning FALSE removes the watch.
        return FALSE;
    }


    gboolean BusListener::onBusMessage(GstBus * inBus, GstMessage * inMessage)
    {
        return TRUE;
    }    
    

    ScopedBusListener::ScopedBusListener(GstElement * inPipeline, const Callback & inCallback) :
        BusListener(inPipeline),
		mCallback(inCallback)
    {
        
    }
    
    
    gboolean ScopedBusListener::onBusMessage(GstBus * inBus, GstMessage * inMessage)
    {
        if (mCallback)
        {
            return mCallback(inBus, inMessage);
        }
        return BusListener::onBusMessage(inBus, inMessage);
    }


} // namespace XULWin
