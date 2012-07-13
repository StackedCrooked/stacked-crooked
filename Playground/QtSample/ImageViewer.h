#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H


#ifndef QT_NO_KEYWORDS
#define QT_NO_KEYWORDS
#endif

#ifndef QT_NO_DEBUG
#define QT_NO_DEBUG
#endif


// INFO: Fixes Clang build error. Maybe this can be removed in the future.
#ifndef QT_NO_STL
#define QT_NO_STL
#endif // QT_NO_STL


#include <QtGui/QtGui>


class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer();

    virtual ~ImageViewer();

    void setImage(const std::string & inFile);

    virtual QSize minimumSizeHint() const;
    QSize sizeHint() const;

private Q_SLOTS:
    void onMenuOpen();
    void onToobarOpen();

private:
    ImageViewer(const ImageViewer&) = delete;
    ImageViewer& operator=(const ImageViewer&) = delete;

    struct Impl;
    Impl * mImpl;
};


#endif // IMAGEVIEWER_H
