#include "ListItemDelegate.h"


ListItemDelegate::ListItemDelegate(QObject * inParent) :
    QAbstractItemDelegate(inParent)
{
}


void ListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->fillRect(option.rect, Qt::white);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QPixmap pixmap = index.data().value<QPixmap>();
    QIcon(pixmap).paint(painter, option.rect);
}


QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem & /* option */, const QModelIndex & inModelIndex) const
{
    QPixmap pixmap = inModelIndex.data().value<QPixmap>();
    return pixmap.size();
}
