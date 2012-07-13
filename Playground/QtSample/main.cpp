#include <QtGui/QtGui>
#include "ImageViewer.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsPixmapItem item(QPixmap("test.jpg"));
    scene.addItem(&item);
    view.show();
    return a.exec();
}
