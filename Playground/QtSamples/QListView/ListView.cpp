#include "ListView.h"


enum
{
    cGridSize = 320,
    cIconSize = 300
};


ListModel::ListModel(QObject * inParent) :
    QAbstractListModel(inParent)
{
}


int ListModel::rowCount(const QModelIndex &) const
{
    return mFiles.size();
}


int ListModel::columnCount(const QModelIndex &) const
{
    return 1;
}


QVariant ListModel::data(const QModelIndex & inModelIndex, int) const
{
    assert(inModelIndex.column() == 0);
    if (inModelIndex.row() < static_cast<int>(mFiles.size()))
    {
        return QVariant(QString(mFiles[inModelIndex.row()].c_str()));
    }
    else
    {
        return QVariant();
    }
}


void ListModel::addFiles(const std::vector<std::string> & inFiles)
{
    mFiles.insert(mFiles.end(), inFiles.begin(), inFiles.end());
}


//std::ostream & operator<<(std::ostream & os, const QRect & inRect)
//{
//    os << "  x: " << inRect.x()
//       << ", y: " << inRect.y()
//       << ", w: " << inRect.width()
//       << ", h: " << inRect.height();
//    return os;
//}


class ItemDelegate : public QAbstractItemDelegate
{
public:
    ItemDelegate(ListView & inListView, QObject * inParent = 0);

    void paint(QPainter * painter, const QStyleOptionViewItem & inOption, const QModelIndex & inModelIndex) const override;

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;

private:
    std::string getFile(const QModelIndex & inModelIndex) const;

    QPixmap getPixmap(const std::string & inFile) const;

    QPixmap getPixmap(const QModelIndex & inModelIndex) const;

    ListView & mListView;
    typedef std::tuple<int, int, std::string> Key;
    typedef std::map<Key, QPixmap> Cache;
    mutable Cache mCache;
};


ItemDelegate::ItemDelegate(ListView & inListView, QObject * inParent) :
    QAbstractItemDelegate(inParent),
    mListView(inListView)
{
}


void ItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & inOption, const QModelIndex & inModelIndex) const
{
    QRect optionRect = inOption.rect;

    // Get a pixmap, using the size and filename as key for caching.
    // The pixmap has correct the aspect ratio, so we can use its
    // size to update the inner rect.
    QPixmap pm = getPixmap(inModelIndex);
    painter->save();
    painter->drawRect(optionRect);
    painter->drawPixmap(QRect(optionRect.x() + (optionRect.width()  - pm.width())/2,
                              optionRect.y() + (optionRect.height() - pm.height())/2,
                              pm.width(),
                              pm.height()), pm);
    painter->restore();
}


QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return QSize(cGridSize, cGridSize);
}


std::string ItemDelegate::getFile(const QModelIndex & inModelIndex) const
{
    QVariant variant = inModelIndex.model()->data(inModelIndex);
    if (variant.isNull())
    {
        return "";
    }
    return variant.toString().toAscii().data();
}


QPixmap ItemDelegate::getPixmap(const std::string & inFile) const
{
    auto w = cIconSize;
    auto h = cIconSize;
    auto key = std::make_tuple(w, h, inFile);
    auto it = mCache.find(key);
    if (it != mCache.end())
    {
        return it->second;
    }

    QPixmap result = QPixmap(inFile.c_str()).scaled(w, h, Qt::KeepAspectRatio);
    mCache.insert(std::make_pair(key, result));
    return result;
}


QPixmap ItemDelegate::getPixmap(const QModelIndex & inModelIndex) const
{
    return getPixmap(getFile(inModelIndex));
}


ListView::ListView(QWidget * inParent) :
    QListView(inParent),
    mListModel(new ListModel(this))
{
    setViewMode(QListView::IconMode);
    setIconSize(QSize(cIconSize, cIconSize));
    setGridSize(QSize(cGridSize, cGridSize));
    setSpacing(0);
    setMovement(QListView::Static);

    setModel(mListModel);

    ItemDelegate * delegate = new ItemDelegate(*this);
    setItemDelegate(delegate);
}
