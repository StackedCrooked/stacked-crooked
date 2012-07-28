#include "ListModel.h"


enum {
    cItemSize = 20
};


ListModel::ListModel()
{
    int row = 0;
    beginInsertRows(QModelIndex(), row, row + 1);
//    pixmaps.insert(row, pixmap);
//    locations.insert(row, location);
    endInsertRows();
}


QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::DecorationRole)
    {
        return QIcon("/Users/francis/QtSDK/Demos/4.7/arthurplugin/flower.jpg");
    }

    return QIcon("/Users/francis/QtSDK/Demos/4.7/arthurplugin/flower.jpg");

    //return QVariant();
}


int ListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return 1;
}
