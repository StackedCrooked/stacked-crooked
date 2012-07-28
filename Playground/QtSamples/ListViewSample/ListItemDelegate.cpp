#include "ListItemDelegate.h"


ListItemDelegate::ListItemDelegate(QObject * inParent) :
    QAbstractItemDelegate(inParent)
{
}


void ListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->fillRect(option.rect, Qt::white);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QIcon icon = index.data().value<QIcon>();
    icon.paint(painter, option.rect);
}


QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem & /* option */, const QModelIndex & /* index */) const
{
    return QSize(400, 400);
}
