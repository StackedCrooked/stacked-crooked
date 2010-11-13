#include "MainWindow.h"
#include <QFileDialog>


const char * cPipeline =
    "filesrc location=\"{{path}}\" ! decodebin2 name=d"
    " d. ! queue ! ffmpegcolorspace ! osxvideosink"
    " d. ! queue ! audioconvert ! audioresample ! autoaudiosink";


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video"), "/", tr("Video Files (*.avi *.mkv *.ogm *.wmv)"));
    QString desc = cPipeline;
    desc.replace("{{path}}", fileName);
    mPipeline.reset(Gst::Pipeline::Parse(desc.toAscii().data()));
    mGstWidget.reset(new Gst::Widget(mPipeline, reinterpret_cast<void*>(winId())));
    Gst::Pipeline::SetState(mPipeline, GST_STATE_PLAYING);
}


MainWindow::~MainWindow()
{
    Gst::Pipeline::SetState(mPipeline, GST_STATE_NULL);
}
