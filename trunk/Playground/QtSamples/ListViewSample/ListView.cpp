#include "ListView.h"
#include "ListModel.h"
#include "ListItemDelegate.h"


namespace {


static const int cItemSize = 240; // TODO: Fix duplication


} // anonymous namespace


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
    return QSize(cItemSize, cItemSize);
}
