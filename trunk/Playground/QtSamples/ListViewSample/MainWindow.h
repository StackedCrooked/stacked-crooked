#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtGui/QMainWindow>
#include "ListView.h"
#include "ListModel.h"
#include "ListItemDelegate.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    ListView * mListView;
    ListModel * mListModel;
    ListItemDelegate * mListItemDelegate;
};


#endif // MAINWINDOW_H
