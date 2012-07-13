#include <QtGui/QtGui>
#include "ImageViewer.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageViewer iv;
    iv.setImage("/Volumes/WesternDigital/Media/Anime/0 Berserk (manga)/Berserk volume 01 [Hawks][HQ]/Berserk_v01_ep01_p018.jpg");
    iv.show();
    return a.exec();
}
