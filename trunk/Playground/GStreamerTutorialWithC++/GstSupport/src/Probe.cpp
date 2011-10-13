#include "Gst/Probe.h"


namespace Gst
{

    BufferProbe::Instances BufferProbe::sInstances;


    BufferProbe::BufferProbe(GstPad * inPad) :
        mPad(inPad),
        mHandler(0)
    {
        mHandler = gst_pad_add_buffer_probe(inPad, G_CALLBACK(BufferProbe::OnProbeBuffer), this);
        sInstances.insert(this);
    }


    BufferProbe::~BufferProbe()
    {
        sInstances.erase(this);
        gst_pad_remove_buffer_probe(mPad, mHandler);
    }
    
    
    void BufferProbe::OnProbeBuffer(GstPad * inPad, GstBuffer * inBuffer, gpointer inData)
    {
        BufferProbe * pThis = static_cast<BufferProbe*>(inData);        
        Instances::iterator it = sInstances.find(pThis);
        if (it != sInstances.end())
        {
            pThis->onProbeBuffer(inPad, inBuffer);
        }
    }


    void BufferProbe::onProbeBuffer(GstPad * inPad, GstBuffer * inBuffer)
    {
        // Do nothing here.
    }    
    

    ScopedBufferProbe::ScopedBufferProbe(GstPad * inPad, const Callback & inCallback) :
        BufferProbe(inPad),
		mCallback(inCallback)
    {
        
    }
    
    
    void ScopedBufferProbe::onProbeBuffer(GstPad * inPad, GstBuffer * inBuffer)
    {
        if (mCallback)
        {
            mCallback(inPad, inBuffer);
        }
    }


} // namespace XULWin
