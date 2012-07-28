#include "MainWindow.h"
#include <functional>


MainWindow::MainWindow(QWidget *inParent) :
    QMainWindow(inParent),
    mCentralWidget()
{
    mCentralWidget = new CentralWidget(this);
    setCentralWidget(mCentralWidget);

    new FileMenu(menuBar(), "&File", *this);
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
