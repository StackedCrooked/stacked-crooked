#include "MainWindow.h"


MainWindow::MainWindow(QWidget *inParent) :
    QMainWindow(inParent),
    mCentralWidget()
{
    mCentralWidget = new CentralWidget(this);
    setCentralWidget(mCentralWidget);
}


MainWindow::~MainWindow()
{
    
}
