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

        GstElement * Create(const std::string & inName);
        GstElement * Parse(const std::string & inPipelineDescription);
        GstState GetState(const GstElement * inElement);
        void SetState(GstElement * inElement, GstState inState);
        void SendEOS(GstElement * inElement);

    } // namespace Pipeline


    namespace Bin
    {
        GstElement * Create(const std::string & inName);
        GstElement * Create(GstElement * inPipeline, const std::string & inName);
        GstElement * Parse(const std::string & inDescription);
        void AddElement(GstElement * inBin, GstElement * inElement);
        void RemoveElement(GstElement * inBin, GstElement * inElement);
        GstElement * GetElementByName(GstBin * inBin, const std::string & inName);

    } // namespace Bin


    namespace Element
    {
        
        GstElement * Create(const std::string & inType, const std::string & inName);
        GstElement * Create(GstElement * inPipeline, const std::string & inType, const std::string & inName);
        GstPad * GetStaticPad(GstElement * inElement, const std::string & inName);
        GstPad * GetRequestPad(GstElement * inElement, const std::string & inName);
        GstPad * GetExistingPadByName(GstElement * inElement, const std::string & inPadName);        
        GstState GetState(const GstElement * inElement);
        void SetState(GstElement * inElement, GstState inState);

        class ScopedStateChanger
        {
        public:
            ScopedStateChanger(GstElement * inElement, GstState inNewState) :
                mElement(inElement),
                mNewState(inNewState)
            {
                mOldState = Element::GetState(mElement);
                if (mOldState != mNewState)
                {
                    SetState(mElement, mNewState);
                }
            }
            
            ~ScopedStateChanger()
            {
                if (mOldState != mNewState)
                {
                    SetState(mElement, mOldState);
                }
            }
            
        private:
            GstElement * mElement;
            GstState mNewState;
            GstState mOldState;
        };
        
        void Link(GstElement * inLHS, GstElement * inRHS);
        void Link(GstElement * a, GstElement * b, GstElement * c);
        void Link(GstElement * a, GstElement * b, GstElement * c, GstElement * d);
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
        std::string ToString(GstState inState);
        std::string ToString(GstMessageType inMessageType);
        std::string ToString(GstPadLinkReturn inPadLinkReturn);
        std::string ToString(GstStreamStatusType inGstStreamStatusType);
        std::string ToString(GstStateChangeReturn inGstStateChangeReturn);

        GstState FromString(const std::string & inState);

    } // namespace Logging


    template<class T>
    void SetProperty(T inObject, const std::string & inKey, const std::string & inValue)
    {
        g_object_set(GST_OBJECT(inObject), inKey.c_str(), inValue.c_str(), NULL);
    }


    //
    // Ref/Unref GstElement
    //
	template<class T>
    inline T * Ref(T * inElement)
    {
        g_object_ref(inElement);
        return inElement;
    }
    
	template<class T>
    inline void Unref(T * inElement)
    { g_object_unref(inElement); }    

    
    //
    // Ref/Unref GstPad
    //
    inline GstPad * Ref(GstPad * inPad)
    {
        g_object_ref(inPad);
        return inPad;
    }
    
    inline void Unref(GstPad * inPad)
    { g_object_unref(inPad); }
    
    
    //
    // Ref/Unref GstCaps
    //
    inline GstCaps * Ref(GstCaps * inCaps)
    {
        gst_caps_ref(inCaps);
        return inCaps;
    }
    
    inline void Unref(GstCaps * inCaps)
    { gst_caps_unref(inCaps); }
    
    
    //
    // ScopedObject
    //
    template<class T>
    class ScopedObject : boost::noncopyable
    {
    public:        
        ScopedObject() :
            mObj(0)
        {
        }
        
        explicit ScopedObject(T * inObj) :
            mObj(inObj)
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
        }
        
        operator bool() const { return mObj != 0; }
        
        T * get() { return mObj; }
        
        const T * get() const { return mObj; }

		operator T* () const { return mObj; }
		operator T* ()  { return mObj; }
        

        T * release()
        {
            T * cpy = mObj;
            mObj = 0;
            return cpy;
        }
        
    private:        
        void destroy()
        {
            if (mObj)
            {
                Unref(mObj);
            }
            mObj = 0;
        }
        
        T * mObj;
    };
    
    
    //
    // SharedObject
    //
    template<class T>
    class SharedObject
    {
    public:        
        explicit SharedObject(T * inObj) :
            mObj(inObj)
        {
        }
        
        
        SharedObject(const SharedObject<T> & rhs) :
            mObj(rhs.mObj)
        {
            Ref(mObj);
        }
        
        
        SharedObject & operator=(const SharedObject<T> & rhs)
        {
            if (this != &rhs)
            {
                if (mObj)
                {
                    Unref(mObj);
                }
                mObj = rhs.mObj;
                Ref(mObj);
            }
        }
        

        ~SharedObject()
        {
            if (mObj)
            {
                Unref(mObj);
            }
        }
        
        void reset(T * inObj)
        {
            if (mObj)
            {
                Unref(mObj);
            }
            mObj = inObj;
        }
        
        operator bool() const { return mObj != 0; }
        
        T * get() { return mObj; }
        
        const T * get() const { return mObj; }
        
    private:        
        T * mObj;
    };


} // namespace Gst


#endif // GSTREAMERSUPPORT_H
