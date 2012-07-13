#include "ImageViewer.h"


struct ImageViewer::Impl
{
    Impl(ImageViewer & inImageViewer) :
        mImageViewer(inImageViewer),
        mScene(&inImageViewer),
        mView(&mScene)
    {
        mToolbar = mImageViewer.addToolBar(tr("File"));
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
        auto h = std::min(desktopRect.height(), mQPixmap.height() + mToolbar->height());
        mImageViewer.resize(w, h);
        mView.resize(w, h);
        mImageViewer.centralWidget()->resize(w, h - mToolbar->height());
    }

    void centerWindow()
    {
        QRect desktopRect = QDesktopWidget().availableGeometry(&mImageViewer);
        auto w = std::min(desktopRect.width(), mQPixmap.width());
        auto h = std::min(desktopRect.height(), mQPixmap.height() + mToolbar->height());
        auto x = (desktopRect.width() - w) / 2;
        auto y = (desktopRect.height() - h) / 2;
        mImageViewer.move(x, y);
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
    QPixmap mQPixmap;

};


ImageViewer::ImageViewer() :
    QMainWindow(),
    mImpl(new Impl(*this))
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(mImpl->mToolbar->addAction("Open"), SIGNAL(triggered()), this, SLOT(openFile()));
    mImpl->centerWindow();
}


ImageViewer::~ImageViewer()
{
    delete mImpl;
}


QSize ImageViewer::sizeHint() const
{
    return QSize(mImpl->mQPixmap.width(), mImpl->mQPixmap.height() + mImpl->mToolbar->height());
}


QSize ImageViewer::minimumSizeHint() const
{
    return QSize(mImpl->mQPixmap.width(), mImpl->mQPixmap.height() + mImpl->mToolbar->height());
}


void ImageViewer::setImage(const std::string & inFile)
{
    mImpl->setImage(inFile);
}


void ImageViewer::openFile()
{
    QString str = QFileDialog::getOpenFileName();
    setImage(str.toUtf8().data());
}
