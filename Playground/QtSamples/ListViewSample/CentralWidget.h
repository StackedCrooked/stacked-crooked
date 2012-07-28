#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H


#include "ListView.h"
#include "ListModel.h"
#include "ListItemDelegate.h"
#include "ImageWidget.h"


class CentralWidget : public QWidget
{
public:
    CentralWidget(QWidget * inParent);

    void addFile(const QString & inFile);

    void setFile(const QString & inFile);

    QSize sizeHint() const;

private:
    ListModel * mListModel;
    ListItemDelegate * mListItemDelegate;
    ListView * mListView;
    ImageWidget * mImageWidget;
};


#endif // CENTRALWIDGET_H
