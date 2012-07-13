#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H


#include <QtGui/QtGui>


class ImageViewer : public QMainWindow
{
public:
    ImageViewer();

    virtual ~ImageViewer();

    void setImage(const std::string & inFile);

private:
    ImageViewer(const ImageViewer&) = delete;
    ImageViewer& operator=(const ImageViewer&) = delete;

    struct Impl;
    Impl * mImpl;
};


#endif // IMAGEVIEWER_H
