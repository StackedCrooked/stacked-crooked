#include "MainWindow.h"


//class FileMenu : public QMenu
//{
//public:
//    FileMenu(QWidget * inParent) :
//        QMenu(inParent)
//    {
//    }

//    void triggered(QAction *action)
//    {
//        action->ge
//    }
//};


MainWindow::MainWindow(QWidget *inParent) :
    QMainWindow(inParent),
    mCentralWidget()
{
    mCentralWidget = new CentralWidget(this);
    setCentralWidget(mCentralWidget);

    //
    // File menu
    //
    QMenu * fileMenu = menuBar()->addMenu("&File");
    QAction * addAction = fileMenu->addAction("&Open...");
    addAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(addAction, SIGNAL(triggered(bool)), this, SLOT(onFileOpen()));
    menuBar()->setNativeMenuBar(true);
}


MainWindow::~MainWindow()
{
    
}


void MainWindow::onFileOpen()
{
    QString str = QFileDialog::getOpenFileName();
    if (!str.isNull())
    {
        mCentralWidget->addFile(str);
    }
}
