#include "ListModel.h"
#include <cassert>


ListModel::ListModel() :
    mFiles(),
    mPixmaps()
{
}


QVariant ListModel::getData(int inRow) const
{
    if (inRow < 0 || inRow >= mPixmaps.size())
    {
        return QVariant();
    }

    return QVariant(mPixmaps.at(inRow));
}


void ListModel::addFile(const QString & inFile)
{
    int row = mFiles.size();
    beginInsertRows(QModelIndex(), row, row);
    mFiles.push_back(inFile);
    mPixmaps.push_back(QPixmap(inFile));
    endInsertRows();
}


QVariant ListModel::data(const QModelIndex &index, int /*role*/) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (index.row() < 0 || index.row() > mPixmaps.size())
    {
        return QVariant();
    }

    return mPixmaps.at(index.row());
}


int ListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    assert(mPixmaps.size() == mFiles.size());
    return mFiles.size();
}
