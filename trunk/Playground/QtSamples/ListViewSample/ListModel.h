#ifndef LISTMODEL_H
#define LISTMODEL_H


#include <QtGui>


class ListModel : public QAbstractListModel
{
public:
    ListModel();

    void addFile(const QString & inFile);

    QVariant getData(int row) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent) const;

private:
    QStringList mFiles;
    QList<QPixmap> mPixmaps;
};


#endif // LISTMODEL_H
