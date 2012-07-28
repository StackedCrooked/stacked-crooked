#ifndef LISTMODEL_H
#define LISTMODEL_H


#include <QtGui>


class ListModel : public QAbstractListModel
{
public:
    ListModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent) const;
};


#endif // LISTMODEL_H
