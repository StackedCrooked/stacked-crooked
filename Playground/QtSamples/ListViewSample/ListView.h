#ifndef LISTVIEW_H
#define LISTVIEW_H


#include <QtGui>


class ListModel;
class ListItemDelegate;


class ListView : public QListView
{
public:
    ListView(QWidget * inParent, ListModel * inListModel, ListItemDelegate * inListItemDelegate);

    QSize sizeHint() const;

private:
    ListModel * mListModel;
    ListItemDelegate * mListItemDelegate;
};


#endif // LISTVIEW_H
