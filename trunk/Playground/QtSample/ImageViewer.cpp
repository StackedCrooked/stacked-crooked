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

    void setImage(const std::string & inPath)
    {
        QPixmap pm(inPath.c_str());
        mScene.addPixmap(pm);

        auto requiredHeight =  pm.height() + mToolbar->height();
        auto requiredWidth = pm.width();
        mImageViewer.resize(std::max(mImageViewer.width(), requiredWidth),
                            std::max(mImageViewer.height(), requiredHeight));

    }

    ImageViewer & mImageViewer;
    QGraphicsScene mScene;
    QGraphicsView mView;
    QToolBar * mToolbar;

};


ImageViewer::ImageViewer() :
    QMainWindow(),
    mImpl(new Impl(*this))
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    resize(1280, 1024);
    QRect desktopRect = QDesktopWidget().availableGeometry();
    move((desktopRect.width() - width()) / 2, (desktopRect.height() - height()) / 2);


    connect(mImpl->mToolbar->addAction("Open"), SIGNAL(triggered()), this, SLOT(openFile()));
}


ImageViewer::~ImageViewer()
{
    delete mImpl;
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
