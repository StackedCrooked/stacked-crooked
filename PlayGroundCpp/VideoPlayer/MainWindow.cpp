#include "MainWindow.h"


#define trace() \
    qDebug("%s:%d", __FUNCTION__, __LINE__)

#define trace_msg(msg) \
    qDebug("%s:%d:%d", __FUNCTION__, __LINE__, (msg))


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    Q_ASSERT(testAttribute(Qt::WA_NativeWindow));

    mPipeline.reset(Gst::Pipeline::Parse("videotestsrc ! osxvideosink sync=false"));
    mGstWidget.reset(new Gst::Widget(mPipeline, reinterpret_cast<void*>(winId())));
    Gst::Pipeline::SetState(mPipeline, GST_STATE_PLAYING);
}


MainWindow::~MainWindow()
{
    Gst::Pipeline::SetState(mPipeline, GST_STATE_NULL);
}
