#include <QtGui/QtGui>
#include "ImageViewer.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageViewer iv;
    iv.show();
    return a.exec();
}
