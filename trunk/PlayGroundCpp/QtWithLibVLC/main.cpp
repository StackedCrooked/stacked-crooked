#include <QtGui/QApplication>
#include "VLCPlayer.h"
#include "MainWindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow wnd;
    wnd.show();
    return a.exec();
}
