#include "ImageWidget.h"


ImageWidget::ImageWidget(QWidget * inParent, const QString & inFile) :
    QWidget(inParent),
    mFile(inFile),
    mPixmap(inFile)
{
}


void ImageWidget::setFile(const QString & inFile)
{
    mFile = inFile;
}


void ImageWidget::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(rect(), Qt::white);

    int w = std::min(width(), mPixmap.width());
    int h = std::min(height(), mPixmap.height());
    int x = (width() - w)/2;
    int y = (height() - h)/2;
    painter.drawPixmap(x, y, w, h, mPixmap);
    painter.end();
}


QSize ImageWidget::sizeHint() const
{
    if (mPixmap.isNull())
    {
        return QSize();
    }

    return mPixmap.size();
}
