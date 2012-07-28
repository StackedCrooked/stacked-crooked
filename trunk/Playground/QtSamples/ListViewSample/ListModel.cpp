#include "ListModel.h"
#include <cassert>


ListModel::ListModel() :
    mFiles()
{
}


QString ListModel::getFile(int inRow) const
{
    if (inRow < 0 || inRow >= mFiles.size())
    {
        return QString();
    }

    return mFiles[inRow];
}


void ListModel::addFile(const QString & inFile)
{
    int row = mFiles.size();
    beginInsertRows(QModelIndex(), row, row);
    mFiles.push_back(inFile);
    endInsertRows();
}


QVariant ListModel::data(const QModelIndex &index, int /*role*/) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    return getFile(index.row());
}


int ListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return mFiles.size();
}
