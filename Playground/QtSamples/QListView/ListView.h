#ifndef LISTVIEW_H
#define LISTVIEW_H


#include <QtGui/QtGui>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <tuple>


#define TRACE //std::cout << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << std::endl;


class ListView;


class ListModel : public QAbstractListModel
{
public:
    ListModel(QObject * inParent = 0);

    int rowCount(const QModelIndex &) const;

    int columnCount(const QModelIndex &) const;

    QVariant data(const QModelIndex & inModelIndex, int) const;

    void addFiles(const std::vector<std::string> & inFiles);

private:
    std::vector<std::string> mFiles;
};


class ListView : public QListView
{
public:
    ListView(QWidget * inParent = 0);

private:
    ListModel * mListModel;
};


#endif // LISTVIEW_H
