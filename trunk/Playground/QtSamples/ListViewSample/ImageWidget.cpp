#include "ImageWidget.h"


ImageWidget::ImageWidget(QWidget * inParent, const QString & inFile) :
    QWidget(inParent),
    mFile(inFile)
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

    QPixmap pm = QPixmap(mFile).scaled(size(), Qt::KeepAspectRatio);
    int w = std::min(width(), pm.width());
    int h = std::min(height(), pm.height());
    int x = (width() - w)/2;
    int y = (height() - h)/2;
    painter.drawPixmap(x, y, w, h, pm);
    painter.end();
}


QSize ImageWidget::sizeHint() const
{
    if (mFile.isNull() || mFile.isEmpty())
    {
        return QSize();
    }

    return QPixmap(mFile).size();
}
