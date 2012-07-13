#include "ImageViewer.h"


struct ImageViewer::Impl
{
    Impl(ImageViewer & inImageViewer) :
        mImageViewer(inImageViewer),
        mScene(&inImageViewer),
        mView(&mScene)
    {
        mToolbar = mImageViewer.addToolBar(tr("File"));
        mStatusbar = mImageViewer.statusBar();
        mImageViewer.statusBar()->setSizeGripEnabled(true);

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

    void centerWindow()
    {
        QRect desktopRect = QDesktopWidget().availableGeometry(&mImageViewer);
        auto w = std::min(desktopRect.width(), mQPixmap.width());
        auto h = std::min(desktopRect.height(), minimumWindowHeight());
        auto x = (desktopRect.width() - w) / 2;
        auto y = (desktopRect.height() - h) / 2;
        mImageViewer.move(x, y);
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
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);


    //
    // File menu
    //

    auto fileMenu = menuBar()->addMenu("&File");
    auto addAction = fileMenu->addAction("&Open...");
    addAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(addAction, SIGNAL(triggered(bool)), this, SLOT(onMenuOpen()));
    menuBar()->setNativeMenuBar(true);


    //
    // Toolbar
    //
    connect(mImpl->mToolbar->addAction("Open"), SIGNAL(triggered()), this, SLOT(onToolbarOpen()));

    mImpl->centerWindow();
}


ImageViewer::~ImageViewer()
{
    delete mImpl;
}


QSize ImageViewer::sizeHint() const
{
    return QMainWindow::sizeHint();
//    static const int cDefaultWidth = 640;
//    static const int cDefaultHeight = 480;
//    return QSize(std::max(cDefaultWidth, mImpl->mQPixmap.width()),
//                 std::max(cDefaultHeight, mImpl->minimumWindowHeight()));
}


QSize ImageViewer::minimumSizeHint() const
{
    return QMainWindow::minimumSizeHint();
    //return QSize(mImpl->mQPixmap.width(), mImpl->minimumWindowHeight());
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
