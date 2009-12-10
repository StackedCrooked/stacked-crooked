#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include "ControlCenterWindow.h"


void center(QWidget &widget)
{
    int x, y;
    int screenWidth;
    int screenHeight;

    QDesktopWidget *desktop = QApplication::desktop();

    screenWidth = desktop->width();
    screenHeight = desktop->height();

    x = (screenWidth - widget.width()) / 2;
    y = (screenHeight - widget.height()) / 2;

    widget.setGeometry(x, y, widget.width(), widget.height());
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("ControlCenter"); // needed for Phonon
    ControlCenterWindow wnd;
    wnd.setWindowTitle("Streamovations Control Center");
    wnd.setGeometry(0, 0, 800, 600);
    center(wnd);
    wnd.show();
    return a.exec();
}
