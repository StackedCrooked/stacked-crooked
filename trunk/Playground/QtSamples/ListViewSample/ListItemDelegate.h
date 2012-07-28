#ifndef LISTITEMDELEGATE_H
#define LISTITEMDELEGATE_H


#include <QtGui>


class ListItemDelegate : public QAbstractItemDelegate
{
public:
    ListItemDelegate(QObject * inParent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;

};


#endif // LISTITEMDELEGATE_H
