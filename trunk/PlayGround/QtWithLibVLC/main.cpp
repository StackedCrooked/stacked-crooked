#include <QtGui/QApplication>
#include "VLCPlayer.h"
#include "MainWindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VLCPlayer p;
    p.resize(640,480);
    p.playFile("rtp://@:4444"); // Replace with what you want to play

    //p.playFile("/home/francis/antipiracyad.avi"); // Replace with what you want to play

    p.show();
    return a.exec();
}
