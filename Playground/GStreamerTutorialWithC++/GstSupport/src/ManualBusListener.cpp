#include "Gst/ManualBusListener.h"
#include "Gst/Logger.h"
#include <cassert>


namespace Gst {


ManualBusListener::ManualBusListener(GstElement * inPipeline) :
    mPipeline(Ref(inPipeline)),
    mBus(gst_pipeline_get_bus(GST_PIPELINE(mPipeline.get())))
{
}


void ManualBusListener::addHandler(GstMessageType inMessageType, const Handler & inHandler)
{
    mHandlers[inMessageType].push_back(inHandler);
}


void ManualBusListener::addFallbackHandler(const Handler & inHandler)
{
    mFallbackHandler = inHandler;
}


void ManualBusListener::checkForMessages()
{
    while (GstMessage * message = gst_bus_pop_filtered(mBus.get(), GST_MESSAGE_ANY))
    {
        HandlersMapping::iterator it = mHandlers.find(message->type);
        if (it != mHandlers.end())
        {
            Handlers & handlers(it->second);
            assert(!handlers.empty());
            for (std::size_t idx = 0; idx != handlers.size(); ++idx)
            {
                Handler & handler(handlers[idx]);
                handler(message);
            }
        }
        else
        {
            if (mFallbackHandler)
            {
                mFallbackHandler(message);
            }
        }

    }
}


} // namespace Gst
