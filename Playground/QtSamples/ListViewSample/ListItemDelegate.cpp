#include "ListItemDelegate.h"
#include <algorithm>


namespace {


static const int cItemSize = 240;


} // anonymous namespace


ListItemDelegate::ListItemDelegate(QObject * inParent) :
    QAbstractItemDelegate(inParent)
{
}


void ListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->fillRect(option.rect, Qt::white);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QString file = index.data().toString();

    QPixmap pixmap = QPixmap(file).scaled(QSize(cItemSize, cItemSize), Qt::KeepAspectRatio);

    int x = option.rect.x() + (cItemSize - pixmap.width()) / 2;
    int y = option.rect.y() + (cItemSize - pixmap.height()) / 2;
    int w = pixmap.width();
    int h = pixmap.height();

    painter->drawPixmap(x, y, w, h, pixmap);
    QIcon().paint(painter, option.rect);
}


QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem & /* option */, const QModelIndex & /*inModelIndex*/) const
{
    return QSize(cItemSize, cItemSize);
}
