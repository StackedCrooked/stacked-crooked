#include "ListView.h"
#include "ListModel.h"
#include "ListItemDelegate.h"


ListView::ListView(QWidget * inParent, ListModel * inListModel, ListItemDelegate * inListItemDelegate) :
    QListView(inParent),
    mListModel(inListModel),
    mListItemDelegate(inListItemDelegate)
{
    setModel(mListModel);
    setItemDelegate(mListItemDelegate);
}


QSize ListView::sizeHint() const
{
    QVariant variant = mListModel->getData(0);
    if (variant.isNull())
    {
        return QListView::sizeHint();
    }

    QPixmap pm = variant.value<QPixmap>();
    return pm.size();
}
