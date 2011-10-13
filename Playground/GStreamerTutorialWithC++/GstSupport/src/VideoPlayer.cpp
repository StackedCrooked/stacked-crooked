#include "Gst/VideoPlayer.h"
#include "Gst/Logger.h"
#include "Gst/MakeString.h"
#include "Gst/NativeWindowImpl.h"
#include <gst/video/video.h>
#include <boost/bind.hpp>
#include <map>
#include <cstdlib>
#include <utility>


#define TRACE // LogDebug(MakeString() << "VideoPlayer::" << __FUNCTION__);

#define PRINT(var) Info() << #var << (var)


#if defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
void GetScreenSize(int & outWidth, int & outHeight)
{
    CGRect mainMonitor = CGDisplayBounds(CGMainDisplayID());
    outWidth = static_cast<int>(0.5 + CGRectGetWidth(mainMonitor));
    outHeight = static_cast<int>(0.5 + CGRectGetHeight(mainMonitor));
}
#endif



namespace Gst {


static std::set<VideoPlayer*> sVideoPlayerInstances;

VideoPlayer::VideoPlayer(Gst::NativeWindow * inWindow, const std::string & inFilePath) :    
    mFilePath(inFilePath),
    mWindow(inWindow),
    mCachedState(GST_STATE_NULL),
    mWidth(0),
    mHeight(0),
    mDuration(0),
    mVideoBoxHorizontalMargin(0),
    mVideoBoxVerticalMargin(0)
{
    sVideoPlayerInstances.insert(this);
    TRACE
    createPlaybin(inFilePath);
    overlay();
    play();
}


VideoPlayer::~VideoPlayer()
{
    if (mPipeline)
    {
        Pipeline::SetState(mPipeline, GST_STATE_NULL);
    }
    
    sVideoPlayerInstances.erase(this);
}


std::pair<int, int> VideoPlayer::getSize() const
{
    return std::make_pair(mWidth, mHeight);
}


GstElement * VideoPlayer::getVideoSink() const
{
    return Bin::Parse("videobox name=videobox ! osxvideosink");
}


void VideoPlayer::createPlaybin(const std::string & inFilePath)
{    
    mPipeline.reset(Element::Create("playbin2", generateUniqueElementName("playbin2")));
    g_object_set(G_OBJECT(mPipeline.get()), "uri", ("file://" + inFilePath).c_str(), NULL);
	mVideoSink.reset(getVideoSink());
    g_object_set(G_OBJECT(mPipeline.get()), "video-sink", mVideoSink.get(), NULL);
    mVideoBox.reset(gst_bin_get_by_name(GST_BIN(mVideoSink.get()), "videobox"));
    mManualBusListener.reset(new Gst::ManualBusListener(mPipeline.get()));
    mManualBusListener->addFallbackHandler(boost::bind(&VideoPlayer::onBusMessage, this, _1));
}


std::string VideoPlayer::generateUniqueElementName(const std::string & inElementName)
{    
    int count = 0;
    Counts::iterator it = mUniqueNames.find(inElementName);
    if (it == mUniqueNames.end())
    {
        mUniqueNames.insert(std::make_pair(inElementName, 0));
    }
    else
    {
        int & countref = it->second;
        countref++;
        count = countref;
    }

    std::stringstream ss;
    ss << inElementName << "_" << count;
    return ss.str();
}


const std::string & VideoPlayer::filePath() const
{
    TRACE
    return mFilePath;
}
    

GstState VideoPlayer::state() const
{
    TRACE
    if (!mPipeline)
    {
        throw std::runtime_error("Getting pipeline state failed because pipeline is not initialized.");
    }
    return mCachedState;
}
    

void VideoPlayer::setState(GstState inState)
{
    TRACE
    if (!mPipeline)
    {
        throw std::runtime_error("Setting pipeline state failed because pipeline is not initialized.");
    }
    Pipeline::SetState(mPipeline.get(), inState);
}
    

void VideoPlayer::play()
{
    TRACE
    if (!mPipeline)
    {
        throw std::runtime_error("'Play' failed because pipeline is not initialized.");
    }
    Pipeline::SetState(mPipeline, GST_STATE_PLAYING);
}


void VideoPlayer::pause()
{
    TRACE
    if (!mPipeline)
    {
        throw std::runtime_error("'Pause' failed because pipeline is not initialized.");
    }
    Pipeline::SetState(mPipeline, GST_STATE_PAUSED);
}


void VideoPlayer::stop()
{
    TRACE
    if (!mPipeline)
    {
        throw std::runtime_error("'Stop' failed because pipeline is not initialized.");
    }
    Pipeline::SetState(mPipeline, GST_STATE_NULL);
}


void VideoPlayer::seek(float inPosition) 
{
    if (inPosition < 0 || inPosition > 1)
    {
        throw std::runtime_error(MakeString() << "Invalid seek position: " << inPosition << ". Must be in [0..1] interval.");
    }
        
    double positionNs = inPosition * static_cast<double>(mDuration);
    if (!gst_element_seek_simple(mPipeline.get(),
                                 GST_FORMAT_TIME,
                                 static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                                 static_cast<gint64>(0.5 + positionNs)))
                                 
    {
        throw std::runtime_error("Seek operation failed.");
    }
}


double VideoPlayer::progress()
{    
    gint64 currentPosition(0);
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_position(mPipeline.get(), &format, &currentPosition);
    
    gint64 theDuration = duration();
    if (!theDuration)
    {
        throw std::runtime_error("Failed to obtain progress because duration could not be obtained.");
    }
    return double(currentPosition) / double(theDuration);
}


gint64 VideoPlayer::duration()
{
    if (mDuration)
    {
        return mDuration;
    }
    
    
    if (mCachedState != GST_STATE_PLAYING)
    {
        throw std::runtime_error("Requesting duration failed because the pipeline is not yet in playing state.");
    }
    
    // Get duration as well
    gint64 result(0);
    GstFormat format = GST_FORMAT_TIME;
    if (!gst_element_query_duration(mPipeline.get(), &format, &result))
    {
        throw std::runtime_error("Failed to get duration.");
    }
    
    mDuration = result;
    return mDuration;
}


void VideoPlayer::overlay()
{
    TRACE
#if defined(_WIN32)
    if (!mWindow)
    {
        LogError("Can't overlay if mWindow is NULL.");
        return;
    }
    mWidget.reset(new Gst::Widget(mPipeline.get(), mWindow->windowId()));
#elif defined(__APPLE__)
    mWidget.reset(new Gst::MacWidget(mPipeline.get()));        
    mWidget->setLogger(boost::bind(LogInfo, _1));
    mWidget->setCallback(boost::bind(&VideoPlayer::onHaveNSWindow, this, _1));
#endif
}


#if defined(__APPLE__)
void VideoPlayer::onHaveNSWindow(void * inChildWnd)
{
    TRACE
    if (!mWindow)
    {
        LogError("The have-ns-window event could not be handled because native window is unintialized.");
        return;
    }
    // Reset the object, so that a proper cleanup of the old view is done.
    mWindow.reset(new Gst::NativeWindow(mWindow->parentWindowId()));
    static_cast<Gst::MacOSXWindow*>(mWindow->impl())->setLogger(boost::bind(LogInfo, _1));
    static_cast<Gst::MacOSXWindow*>(mWindow->impl())->setNSView(inChildWnd);
}
#endif
    

void VideoPlayer::GetWidthAndHeight(double inAspectRatio, int & ioWidth, int & ioHeight)
{      
    double adjustedWidth = static_cast<double>(ioHeight) * inAspectRatio;
    if (adjustedWidth <= ioWidth)
    {
        ioWidth = static_cast<int>(0.5 + adjustedWidth);
    }
    else
    {
        ioHeight = static_cast<int>(0.5 + static_cast<double>(ioWidth) / inAspectRatio);
    }
}


void ToggleVideoPlayerFullScreen()
{
    Info() << "ToggleVideoPlayerFullScreen";
    try
    {
        if (!sVideoPlayerInstances.empty())
        {
            (*sVideoPlayerInstances.begin())->toggleFullScreen();
        }
    }
    catch (const std::exception & exc)
    {
        LogError("Failed to set full screen mode. Reason: " + std::string(exc.what()));
    }
}


void VideoPlayer::toggleFullScreen()
{
    Info() << "VideoPlayer::toggleFullScreen";
    setFullScreen(!mWindow->isFullScreen());
}


void VideoPlayer::setFullScreen(bool inFullScreen)
{
    Info() << "VideoPlayer::setFullScreen";
    mWindow->setFullScreen(inFullScreen);
    
    if (!mWidth || !mHeight)
    {
        throw std::runtime_error("Video width and height must be known before going full-screen.");
    }

    //
    // Set correct aspect ratio by adding black borders using the videobox element.
    //
    int horizontalMargin(0); 
    int verticalMargin(0);
     
    if (inFullScreen)
    {
        int screenWidth(0), screenHeight(0);
        GetScreenSize(screenWidth, screenHeight);
        if (!screenWidth || !screenHeight)
        {
            throw std::runtime_error("Failed to get screen size.");
        }
        
        double screenAspectRatio = static_cast<double>(screenWidth)/static_cast<double>(screenHeight);
        double movieAspectRatio = static_cast<double>(mWidth)/static_cast<double>(mHeight);
        
        if (movieAspectRatio > screenAspectRatio)
        {
            double growthFactor = screenWidth / mWidth;
            double fullScreenHeight = growthFactor * static_cast<double>(mHeight);
            double remainder = (static_cast<double>(screenHeight) - fullScreenHeight) / growthFactor;
            verticalMargin = static_cast<int>(0.5 + remainder);
        }
        else
        {
            double growthFactor = screenHeight / mHeight;
            double fullScreenWidth = growthFactor * static_cast<double>(mWidth);
            double remainder = (static_cast<double>(screenWidth) - fullScreenWidth) / growthFactor;
            horizontalMargin = static_cast<int>(0.5 + remainder);            
        }
    }    

    if (mVideoBoxHorizontalMargin != horizontalMargin || mVideoBoxVerticalMargin != verticalMargin)
    {                    
        //Element::SetState(mVideoBox.get(), GST_STATE_PAUSED);
        Element::ScopedStateChanger stateChanger(mVideoBox.get(), GST_STATE_PAUSED);
        if (mVideoBoxHorizontalMargin != horizontalMargin)
        {
            int left = horizontalMargin ? -horizontalMargin/2 : 0;
            int right = left;
            g_object_set(G_OBJECT(mVideoBox.get()), "left", gint(left), NULL);
            g_object_set(G_OBJECT(mVideoBox.get()), "right", gint(right), NULL);
            mVideoBoxHorizontalMargin = horizontalMargin;
        }                    
        
        if (mVideoBoxVerticalMargin != verticalMargin)
        {
            int top = verticalMargin ? -verticalMargin/2 : 0;
            int bottom = top;
            g_object_set(G_OBJECT(mVideoBox.get()), "top", gint(top), NULL);
            g_object_set(G_OBJECT(mVideoBox.get()), "bottom", gint(bottom), NULL);
            mVideoBoxVerticalMargin = verticalMargin;
        }
        //Element::SetState(mVideoBox.get(), GST_STATE_PLAYING);
    }
}

    
bool VideoPlayer::moveWindow(int x, int y, int width, int height)
{
    if (mWindow)
    {
        if (mWidth == 0 && mHeight == 0)
        {
            mWindow->move(x, y, width, height);
        }
        else
        {
            int newWidth(width);
            int newHeight(height);
            double aspectRatio = static_cast<double>(mWidth) / static_cast<double>(mHeight);
            GetWidthAndHeight(aspectRatio, newWidth, newHeight);
            int horizontalMargin = (newWidth < width) ? (width - newWidth) : 0;
            int verticalMargin = (newHeight < height) ? (height - newHeight) : 0;
            mWindow->move(horizontalMargin/2, verticalMargin/2, newWidth, newHeight);
        }
        return true;
    }
    return false;
}


void VideoPlayer::setWindowHidden(bool inHidden)
{
    if (mWindow)
    {
        mWindow->setHidden(inHidden);
    }
}


void VideoPlayer::redrawWindow()
{
    TRACE
    if (mWindow)
    {
        mWindow->redraw();
    }
}


void VideoPlayer::onBusMessage(GstMessage * inMessage)
{    
    TRACE
    switch (inMessage->type)
    {
        case GST_MESSAGE_INFO:
        {
            LogInfo(Logging::GetLogMessage(inMessage));
            break;
        }
        case GST_MESSAGE_WARNING:
        {
            LogWarning(Logging::GetLogMessage(inMessage));
            break;
        }
        case GST_MESSAGE_ERROR:
        {
            LogError(Logging::GetLogMessage(inMessage));
            break;
        }        
        case GST_MESSAGE_STATE_CHANGED:
        {
            onStateChanged(inMessage);
            break;
        }
        case GST_MESSAGE_TAG:
        {
            onTagMessage(inMessage);
            break;
        }
        case GST_MESSAGE_DURATION:
        {
            onDurationMessage(inMessage);
        }
        default:
        {
            // Not interested.
            break;
        }
    }
}


void VideoPlayer::onTagMessage(GstMessage * inMessage)
{
    GstTagList * tags(0);
    GstPad * pad(0);
    gst_message_parse_tag_full(inMessage, &pad, &tags);
    
    if (tags)
    {
        gst_tag_list_free(tags);
    }
    
    if (!pad)
    {
        return;
    }
    
    GstCaps * caps(gst_pad_get_caps(pad));
    if (!caps)
    {
        return;
    }

    std::size_t numStructs = gst_caps_get_size(caps);
    for (std::size_t idx = 0; idx != numStructs; ++idx)
    {
        GstStructure * structure = gst_caps_get_structure(caps, idx);
        if (gst_structure_has_field(structure, "width") && gst_structure_has_field(structure, "height"))
        {
            mWidth = g_value_get_int(gst_structure_get_value(structure, "width"));
            mHeight = g_value_get_int(gst_structure_get_value(structure, "height"));
        }
    }
    Unref(caps);
    Unref(pad);
}


void VideoPlayer::onDurationMessage(GstMessage * inMessage)
{
    mDuration = duration();
}


void VideoPlayer::checkForMessages()
{
    if (mManualBusListener)
    {
        mManualBusListener->checkForMessages();
    }
}


void VideoPlayer::onStateChanged(GstMessage * inMessage)
{
    if (GST_ELEMENT(GST_MESSAGE_SRC(inMessage)) == mPipeline.get())
    {
        GstState oldState;
        gst_message_parse_state_changed(inMessage, &oldState, &mCachedState, NULL);
        if (mCachedState == GST_STATE_PLAYING)
        {
            mDuration = duration();
        }
    }
}


} // namespace Gst
