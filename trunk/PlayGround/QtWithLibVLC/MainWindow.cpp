#include "MainWindow.h"


MainWindow::MainWindow() :
    mDesktopVideo(0),
    mPresenterVideo(0)
{
    mDesktopVideo = new VLCPlayer(this, 640, 480);
    mDesktopVideo->playFile("rtp://@:4444");

    mPresenterVideo = new VLCPlayer(this, 320, 240);
    mPresenterVideo->playFile("/home/francis/antipiracyad.avi");

    QHBoxLayout * hbox = new QHBoxLayout();
    hbox->addWidget(mDesktopVideo, 1, Qt::AlignTop);
    hbox->addWidget(mPresenterVideo, 1, Qt::AlignTop);
    setLayout(hbox);
}

MainWindow::~MainWindow()
{

}
