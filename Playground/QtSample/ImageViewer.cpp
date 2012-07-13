#include "ImageViewer.h"


struct ImageViewer::Impl
{
    Impl(ImageViewer & inImageViewer) :
        mImageViewer(inImageViewer),
        mScene(&inImageViewer),
        mView(&mScene)
    {
        mImageViewer.resize(1280, 1024);
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
        mScene.addPixmap(QPixmap(inPath.c_str()));
    }

    ImageViewer & mImageViewer;
    QGraphicsScene mScene;
    QGraphicsView mView;
};


ImageViewer::ImageViewer() :
    QMainWindow(),
    mImpl(new Impl(*this))
{
}


ImageViewer::~ImageViewer()
{
    delete mImpl;
}


void ImageViewer::setImage(const std::string & inFile)
{
    mImpl->setImage(inFile);
}
