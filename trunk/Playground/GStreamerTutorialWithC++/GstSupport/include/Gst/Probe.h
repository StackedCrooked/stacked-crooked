#ifndef GSTPROBE_H_INCLUDED
#define GSTPROBE_H_INCLUDED


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
     * BufferProbe
     *
     * Helper class for probing a buffer. Takes care of all management.
     * Inherit onProbeBuffer to implement the callback.
     */
    class BufferProbe : boost::noncopyable
    {
    public:
        BufferProbe(GstPad * inPad);

        virtual ~BufferProbe();            
        
    protected:
        virtual void onProbeBuffer(GstPad * inPad, GstBuffer * inBuffer);

    private:
        static void OnProbeBuffer(GstPad * inPad, GstBuffer * inBuffer, gpointer inData);

        GstPad * mPad;
        gulong mHandler;

        typedef std::set<BufferProbe*> Instances;
        static Instances sInstances;
    };


    /**
     * ScopedBufferProbe
     *
     * Enables probing a buffer using a boost function object as callback.
     */
    class ScopedBufferProbe : public BufferProbe
    {
    public:
        // The callback implementation should normally return TRUE.
        // If can unregister itself as listener by returning FALSE.
        typedef boost::function<void(GstPad *, GstBuffer *)> Callback;

        ScopedBufferProbe(GstPad * inPad, const Callback & inCallback);

    protected:
        virtual void onProbeBuffer(GstPad * inPad, GstBuffer * inBuffer);

        Callback mCallback;
    };

} // namespace Gst


#endif // GSTPROBE_H_INCLUDED
