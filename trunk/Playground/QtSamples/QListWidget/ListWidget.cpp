//#include "ListWidget.h"


//enum
//{
//    cGridSize = 320,
//    cIconSize = 300
//};

////std::ostream & operator<<(std::ostream & os, const QRect & inRect)
////{
////    os << "  x: " << inRect.x()
////       << ", y: " << inRect.y()
////       << ", w: " << inRect.width()
////       << ", h: " << inRect.height();
////    return os;
////}


//class ItemDelegate : public QAbstractItemDelegate
//{
//public:
//    ItemDelegate(ListWidget & inListWidget, QObject * inParent = 0);

//    void paint(QPainter * painter, const QStyleOptionViewItem & inOption, const QModelIndex & inModelIndex) const override;

//    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;

//private:
//    std::string getFile(const QModelIndex & inModelIndex) const;

//    QPixmap getPixmap(const std::string & inFile) const;

//    QPixmap getPixmap(const QModelIndex & inModelIndex) const;

//    ListWidget & mListWidget;
//    typedef std::tuple<int, int, std::string> Key;
//    typedef std::map<Key, QPixmap> Cache;
//    mutable Cache mCache;
//};


//ItemDelegate::ItemDelegate(ListWidget & inListWidget, QObject * inParent) :
//    QAbstractItemDelegate(inParent),
//    mListWidget(inListWidget)
//{
//}


//void ItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & inOption, const QModelIndex & inModelIndex) const
//{
//    QRect optionRect = inOption.rect;

//    // Get a pixmap, using the size and filename as key for caching.
//    // The pixmap has correct the aspect ratio, so we can use its
//    // size to update the inner rect.
//    QPixmap pm = getPixmap(inModelIndex);
//    painter->save();
//    painter->drawRect(optionRect);
//    painter->drawPixmap(QRect(optionRect.x() + (optionRect.width()  - pm.width())/2,
//                              optionRect.y() + (optionRect.height() - pm.height())/2,
//                              pm.width(),
//                              pm.height()), pm);
//    painter->restore();
//}


//QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
//{
//    return QSize(cGridSize, cGridSize);
//}


//std::string ItemDelegate::getFile(const QModelIndex & inModelIndex) const
//{
//    QVariant variant = inModelIndex.model()->data(inModelIndex);
//    if (variant.isNull())
//    {
//        return "";
//    }
//    return variant.toString().toAscii().data();
//}


//QPixmap ItemDelegate::getPixmap(const std::string & inFile) const
//{
//    auto w = cIconSize;
//    auto h = cIconSize;
//    auto key = std::make_tuple(w, h, inFile);
//    auto it = mCache.find(key);
//    if (it != mCache.end())
//    {
//        return it->second;
//    }

//    QPixmap result = QPixmap(inFile.c_str()).scaled(w, h, Qt::KeepAspectRatio);
//    mCache.insert(std::make_pair(key, result));
//    return result;
//}


//QPixmap ItemDelegate::getPixmap(const QModelIndex & inModelIndex) const
//{
//    return getPixmap(getFile(inModelIndex));
//}


//ListWidget::ListWidget(QWidget * inParent) :
//    QListWidget(inParent),
//    mListModel(new ListModel(this))
//{
//    setViewMode(QListWidget::IconMode);
//    setIconSize(QSize(cIconSize, cIconSize));
//    setGridSize(QSize(cGridSize, cGridSize));
//    setSpacing(0);
//    setMovement(QListWidget::Static);

//    setModel(mListModel);

//    ItemDelegate * delegate = new ItemDelegate(*this);
//    setItemDelegate(delegate);
//}
