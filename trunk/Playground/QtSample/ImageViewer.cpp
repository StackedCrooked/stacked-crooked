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

    void adjustWindowSize()
    {
        QRect desktopRect = QDesktopWidget().availableGeometry(&mImageViewer);

        auto w = std::min(desktopRect.width(), mQPixmap.width());
        auto h = std::min(desktopRect.height(), minimumWindowHeight());
        mImageViewer.resize(w, h);

        mView.resize(w, h);
        mView.setMinimumSize(w, h);

        auto & cw = *mImageViewer.centralWidget();
        cw.resize(w, h - mToolbar->height());
        cw.setMinimumSize(w, h);

        mScene.setSceneRect(cw.rect());

        mImageViewer.setMinimumSize(w, minimumWindowHeight());
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
        adjustWindowSize();
        centerWindow();
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
    connect(mImpl->mToolbar->addAction("Open"), SIGNAL(triggered()), this, SLOT(onToobarOpen()));

    mImpl->centerWindow();
}


ImageViewer::~ImageViewer()
{
    delete mImpl;
}


QSize ImageViewer::sizeHint() const
{
    return QSize(mImpl->mQPixmap.width(), mImpl->minimumWindowHeight());
}


QSize ImageViewer::minimumSizeHint() const
{
    return QSize(mImpl->mQPixmap.width(), mImpl->minimumWindowHeight());
}


void ImageViewer::setImage(const std::string & inFile)
{
    mImpl->setImage(inFile);
}


void ImageViewer::onMenuOpen()
{
    QString str = QFileDialog::getOpenFileName();
    setImage(str.toUtf8().data());
}


void ImageViewer::onToobarOpen()
{
    QString str = QFileDialog::getOpenFileName();
    setImage(str.toUtf8().data());
}
