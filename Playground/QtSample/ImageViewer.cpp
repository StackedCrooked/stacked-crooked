#include "ImageViewer.h"


struct RememberWindowPosition : QSettings
{
    RememberWindowPosition(QWidget & inWidget) :
        QSettings("Mesmerizing", "Charm"),
        mWidget(inWidget)
    {
        mWidget.restoreGeometry(value(Key()).toByteArray());
    }

    ~RememberWindowPosition()
    {
        setValue(Key(), mWidget.saveGeometry());
    }

    static const char * Key() { return "geometry"; }

    QWidget & mWidget;
};


struct ImageViewer::Impl : RememberWindowPosition
{
    Impl(ImageViewer & inImageViewer) :
        RememberWindowPosition(inImageViewer),
        mImageViewer(inImageViewer),
        mScene(&inImageViewer),
        mView(&mScene)
    {
        mToolbar = mImageViewer.addToolBar(tr("File"));
        mStatusbar = mImageViewer.statusBar();
        mImageViewer.statusBar()->setSizeGripEnabled(true);

        // We make a 3x3 grid layout where each row and column pushes equally against the middle square.
        // This way it is centered horizontally and vertically.
        QGridLayout * layout = new QGridLayout;
        layout->setRowStretch(0, 1);
        layout->setRowStretch(1, 0);
        layout->setRowStretch(2, 1);
        layout->setColumnStretch(0, 1);
        layout->setColumnStretch(1, 0);
        layout->setColumnStretch(2, 1);

        // Add Graphics view in the middle.
        layout->addWidget(&mView, 1, 1, 0, 0);

        mImageViewer.setCentralWidget(&mView);

        mView.show();
        mScene.update();
    }

    ~Impl()
    {

    }

    QWidget & centralWidget()
    {
        return *mImageViewer.centralWidget();
    }

    int minimumWindowHeight()
    {
        return mStatusbar->height() + mQPixmap.height() + mToolbar->height();
    }

    void setImage(const std::string & inPath)
    {
        mScene.clear();
        mQPixmap.load(inPath.c_str());
        mScene.addPixmap(mQPixmap);
    }

    ImageViewer & mImageViewer;
    QGraphicsScene mScene;
    QGraphicsView mView;
    QToolBar * mToolbar;
    QStatusBar * mStatusbar;
    QPixmap mQPixmap;
};


ImageViewer::ImageViewer() :
    QMainWindow(),
    mImpl(new Impl(*this))
{
    // File menu
    auto fileMenu = menuBar()->addMenu("&File");
    auto addAction = fileMenu->addAction("&Open...");
    addAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(addAction, SIGNAL(triggered(bool)), this, SLOT(onMenuOpen()));
    menuBar()->setNativeMenuBar(true);

    // Toolbar
    connect(mImpl->mToolbar->addAction("Open"), SIGNAL(triggered()), this, SLOT(onToolbarOpen()));
}


ImageViewer::~ImageViewer()
{
    delete mImpl;
}


QSize ImageViewer::sizeHint() const
{
    return QMainWindow::sizeHint();
}


QSize ImageViewer::minimumSizeHint() const
{
    return QMainWindow::minimumSizeHint();
}


void ImageViewer::setImage(const std::string & inFile)
{
    mImpl->setImage(inFile);
}


void ImageViewer::onMenuOpen()
{
    QString str = QFileDialog::getOpenFileName();
    if (!str.isNull())
    {
        setImage(str.toUtf8().data());
    }
}


void ImageViewer::onToolbarOpen()
{
    QString str = QFileDialog::getOpenFileName();
    if (!str.isNull())
    {
        setImage(str.toUtf8().data());
    }
}
