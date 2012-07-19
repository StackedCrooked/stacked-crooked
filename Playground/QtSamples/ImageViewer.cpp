#include "ImageViewer.h"


class RememberWindowPosition
{
public:
    RememberWindowPosition(QWidget & inWidget) :
        mSettings("Mesmerizing", "Charm"),
        mWidget(inWidget)
    {
        mWidget.restoreGeometry(mSettings.value(Key()).toByteArray());
    }

    ~RememberWindowPosition()
    {
        mSettings.setValue(Key(), mWidget.saveGeometry());
    }

private:
    RememberWindowPosition(const RememberWindowPosition&) = delete;
    RememberWindowPosition& operator=(const RememberWindowPosition&) = delete;

    static const char * Key() { return "geometry"; }

    QSettings mSettings;
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
        mToolbar = mImageViewer.addToolBar("The Toolbar");
        mToolbar->setMovable(false);

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
        return mQPixmap.height() + mToolbar->height();
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
#define MESMERIZE_TOOLBAR_BUTTON(Name) \
    auto the##Name##Action = mImpl->mToolbar->addAction(#Name); \
    the##Name##Action->setDisabled(true); \
    connect(the##Name##Action, SIGNAL(triggered()), this, SLOT(onToolbarBack()));


    MESMERIZE_TOOLBAR_BUTTON(Back);
    MESMERIZE_TOOLBAR_BUTTON(Forward);
    MESMERIZE_TOOLBAR_BUTTON(Skip);
    MESMERIZE_TOOLBAR_BUTTON(Charming);
    MESMERIZE_TOOLBAR_BUTTON(Hot);
    MESMERIZE_TOOLBAR_BUTTON(Mesmerizing);

//    connect(mImpl->mToolbar->addAction("Back"), SIGNAL(triggered()), this, SLOT(onToolbarBack()));
//    connect(mImpl->mToolbar->addAction("Forward"), SIGNAL(triggered()), this, SLOT(onToolbarForward()));
//    connect(mImpl->mToolbar->addAction("Skip"),         SIGNAL(triggered()), this, SLOT(onToolbarSkip()));
//    connect(mImpl->mToolbar->addAction("Charming"),     SIGNAL(triggered()), this, SLOT(onToolbarCharming()));
//    connect(mImpl->mToolbar->addAction("Hot"),          SIGNAL(triggered()), this, SLOT(onToolbarHot()));
//    connect(mImpl->mToolbar->addAction("Mesmerizing"),  SIGNAL(triggered()), this, SLOT(onToolbarMesmerizing()));
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


void ImageViewer::onToolbarBack()
{
    // TODO: Show previous image. (Rotate)
}


void ImageViewer::onToolbarForward()
{
    // TODO: Show next image. (Rotate)
}


void ImageViewer::onToolbarSkip()
{
    // TODO: Implement!
}


void ImageViewer::onToolbarCharming()
{
    // TODO: Implement!
}


void ImageViewer::onToolbarHot()
{
    // TODO: Implement!
}


void ImageViewer::onToolbarMesmerizing()
{
    // TODO: Implement!
}

