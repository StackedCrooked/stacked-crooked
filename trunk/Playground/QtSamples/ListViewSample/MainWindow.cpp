#include "MainWindow.h"
#include <cassert>
#include <functional>


MainWindow::MainWindow(QWidget *inParent) :
    QMainWindow(inParent),
    mCentralWidget(),
    mFileOpenEvent()
{
    mCentralWidget = new CentralWidget(this);
    setCentralWidget(mCentralWidget);

    Menu * fileMenu = new Menu(menuBar(), "&File", *this);
    mFileOpenEvent = fileMenu->addMenuItem("&File", QKeySequence("Ctrl+O"));
}


MainWindow::~MainWindow()
{
}


void MainWindow::onMenuTriggered(Menu &, MenuEvent& inMenuEvent)
{
    if (&inMenuEvent == mFileOpenEvent)
    {
        QString str = QFileDialog::getOpenFileName();
        if (!str.isNull())
        {
            mCentralWidget->addFile(str);
        }
    }
    else
    {
        assert(false);
    }
}
