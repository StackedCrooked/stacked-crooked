#include "ListItemDelegate.h"


ListItemDelegate::ListItemDelegate(QObject * inParent) :
    QAbstractItemDelegate(inParent)
{
}

void ListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    (void)painter;
    (void)option;
    (void)index;

    painter->fillRect(option.rect, QColor(0, 0, 255));

    QIcon icon = index.data().value<QIcon>();
    icon.paint(painter, option.rect);

}


QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem & /* option */, const QModelIndex & /* index */) const
{
    return QSize(40, 40);
}
