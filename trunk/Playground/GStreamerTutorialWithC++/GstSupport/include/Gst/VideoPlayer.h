#ifndef GST_GSTVIDEOPLAYER_H_INCLUDED
#define GST_GSTVIDEOPLAYER_H_INCLUDED


#include "Gst/Support.h"
#include "Gst/ManualBusListener.h"
#include "Gst/NativeWindow.h"
#if defined(__APPLE__)
#include "Gst/MacWidget.h"
#include "Gst/MacOSXWindow.h"
#else
#include "Gst/Widget.h"
#endif
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>
#include <string>


namespace Gst {

	
class VideoPlayer
{
public:
	VideoPlayer(Gst::NativeWindow * inWindow, const std::string & inFilePath);
    
    ~VideoPlayer();
    
    const std::string & filePath() const;
    
    GstState state() const;
    
    void play();
    
    void pause();
    
    void stop();
    
    // inPosition must be in [0..1] range
    void seek(float inPosition);
    
    double progress();
    
    gint64 duration();
    
    void setFullScreen(bool inFullscreen);
    
    void toggleFullScreen();
    
    bool moveWindow(int x, int y, int width, int height);
    
    void setWindowHidden(bool inHidden);
    
    void redrawWindow();
    
    void overlay();
    
    // Checks for bus messages.
    void checkForMessages();
    
    static void GetWidthAndHeight(double inAspectRatio, int & ioWidth, int & ioHeight);

    std::pair<int, int> getSize() const;
	
private:    
    void createPlaybin(const std::string & inFilePath);
    GstElement * getVideoSink() const;
    std::string generateUniqueElementName(const std::string & inElementName);

    void onBusMessage(GstMessage * inMessage);
    void onStateChanged(GstMessage * inMessage);
    void onTagMessage(GstMessage * inMessage);
    void onDurationMessage(GstMessage * inMessage);
    
    void setState(GstState inState);
    
    #if defined(__APPLE__)
    void onHaveNSWindow(void * inChildWnd);
    #endif
    
    std::string mFilePath;
    ScopedObject<GstElement> mPipeline;
    ScopedObject<GstElement> mVideoSink;
    ScopedObject<GstElement> mVideoBox;
    int mVideoBoxHorizontalMargin;
    int mVideoBoxVerticalMargin;
    boost::scoped_ptr<NativeWindow> mWindow;
    
    #if defined(__APPLE__)
    boost::scoped_ptr<MacWidget> mWidget;
    #else
    boost::scoped_ptr<Widget> mWidget;
    #endif
    
    GstElement * mFileSrc;
    GstElement * mDecodeBin;
    std::vector<GstPad*> mVideoStreams;
    std::vector<GstPad*> mAudioStreams;
    std::vector<GstPad*> mSubtitleStreams;
	boost::scoped_ptr<ManualBusListener> mManualBusListener;
    
    GstState mCachedState;
    
    int mWidth;
    int mHeight;
    mutable gint64 mDuration;
    
    // Helper for element naming.
    typedef std::map<std::string, int> Counts;
    Counts mUniqueNames;
};


} // namespace Gst


#endif // GST_GSTVIDEOPLAYER_H_INCLUDED
