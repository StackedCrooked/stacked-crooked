#ifndef XPCOM_VIDEO_NATIVEWINDOWIMPL_H_INCLUDED
#define XPCOM_VIDEO_NATIVEWINDOWIMPL_H_INCLUDED


#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <string>


namespace Gst
{

    class NativeWindowImpl : boost::noncopyable
    {
    public:
        virtual ~NativeWindowImpl() {}

        virtual void move(int x, int y, int w, int h) = 0;

        virtual void setHidden(bool inHidden) = 0;
        
        virtual void toggleFullScreen() = 0;

        virtual bool isFullScreen() const = 0;

        virtual void setFullScreen(bool inFullscreen) = 0;

        virtual void redraw() = 0;

        virtual void * parentWindowId() = 0;

        virtual void * windowId() = 0;
        
        typedef boost::function<void(const std::string &)> Logger;
        void setLogger(const Logger & inLogger) { mLogger = inLogger; }       

    protected:
        void logDebug(const std::string & inMsg)
        {
            if (mLogger)
            {
                mLogger("DEBUG: " + inMsg);
            }
        }
        
        
        void logError(const std::string & inMsg)
        {
            if (mLogger)
            {
                mLogger("ERROR: " + inMsg);
            }
        }

    private:
        Logger mLogger;
    };

} // namespace Gst


#endif // XPCOM_VIDEO_NATIVEWINDOWIMPL_H_INCLUDED
