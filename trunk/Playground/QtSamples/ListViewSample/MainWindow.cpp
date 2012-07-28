#include "MainWindow.h"
#include <functional>


MainWindow::MainWindow(QWidget *inParent) :
    QMainWindow(inParent),
    mCentralWidget()
{
    mCentralWidget = new CentralWidget(this);
    setCentralWidget(mCentralWidget);

    FileMenu::Create(menuBar(), "&File", *this);
    menuBar()->setNativeMenuBar(true);
}


MainWindow::~MainWindow()
{
}


void MainWindow::onFileMenuTriggered(FileMenu&)
{
    QString str = QFileDialog::getOpenFileName();
    if (!str.isNull())
    {
        mCentralWidget->addFile(str);
    }
}
