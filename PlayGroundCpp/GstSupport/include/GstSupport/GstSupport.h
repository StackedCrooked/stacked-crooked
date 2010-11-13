#ifndef GSTREAMERSUPPORT_H
#define GSTREAMERSUPPORT_H


extern "C"
{
    #include <gst/gst.h>
    #include <glib.h>
}
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <map>
#include <string>
#include <sstream>
#include <vector>


namespace Gst
{

    namespace Pipeline
    {

        GstElement * Create(const char * inName);
        GstElement * Parse(const std::string & inPipelineDescription);
        GstState GetState(GstElement * inElement);
        void SetState(GstElement * inElement, GstState inState);
        GstElement * GetElementByName(GstElement * inPipeline, const char * inName);
        void SendEOS(GstElement * inElement);

    } // namespace Pipeline


    namespace Bin
    {
        GstElement * GetElementByName(GstBin * inBin, const char * inName);

    } // namespace Bin


    namespace Element
    {
        
        GstElement * Create(const char * inType, const char * inName);
        GstPad * GetStaticPad(GstElement * inElement, const char * inName);
        GstPad * GetRequestPad(GstElement * inElement, const char * inName);
        GstPad * GetExistingPadByName(GstElement * inElement, const std::string & inPadName);
        void Link(GstElement * inLHS, GstElement * inRHS);
        void GetDeviceNames(GstElement * inCaptureSrcEl, std::vector<std::string> & outDeviceNames);

    } // namespace Element


    namespace Pad
    {   

        void Link(GstPad * inLHS, GstPad * inRHS);
        bool IsAudioPad(GstPad * inPad);
        bool IsVideoPad(GstPad * inPad);

    } // namespace Pad


    namespace Caps
    {

        std::string GetName(GstCaps * inCaps);

    } // namespace Caps


    namespace Logging
    {
    
        bool IsLogMessage(GstMessage * msg);
        std::string GetLogMessage(GstMessage * msg);
        const char * ToString(GstState inState);
        const char * ToString(GstMessageType inMessageType);
        const char * ToString(GstPadLinkReturn inPadLinkReturn);
        const char * ToString(GstStreamStatusType inGstStreamStatusType);
        const char * ToString(GstStateChangeReturn inGstStateChangeReturn);

        GstState FromString(const std::string & inState);

    } // namespace Logging


    template<class T>
    void SetProperty(T inObject, const std::string & inKey, const std::string & inValue)
    {
        g_object_set(GST_OBJECT(inObject), inKey.c_str(), inValue.c_str(), NULL);
    }


    template<class T>
    class ScopedObject : boost::noncopyable
    {
    public:
        ScopedObject() :
            mObj(0),
            mOwns(false)
        {
        }

        explicit ScopedObject(T * inObj) :
            mObj(inObj),
            mOwns(true)
        {
        }

        ~ScopedObject()
        {
            destroy();
        }

        void reset(T * inObj)
        {
            destroy();
            mObj = inObj;
            mOwns = true;
        }

        operator const T * () const
        {
            return mObj;
        }

        operator T * ()
        {
            return mObj;
        }

        T * get() { return mObj; }

        const T * get() const { return mObj; }
        
        T * release() { mOwns = false; return mObj; }

    private:        
        void destroy()
        {
            if (mOwns && mObj)
            {
                g_object_unref(mObj);
            }
            mObj = 0;
            mOwns = false;
        }

        T * mObj;
        bool mOwns; // needed for the release() functionality.
    };



} // namespace Gst


#endif // GSTREAMERSUPPORT_H
