#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H


#include <QtGui>


class ImageWidget : public QWidget
{
public:
    ImageWidget(QWidget * inParent, const QString & inFile = QString());

    void setFile(const QString & inFile);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *);

private:
    QString mFile;
};


#endif // IMAGEWIDGET_H
