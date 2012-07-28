#include "MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mListView(),
    mListModel(),
    mListItemDelegate()
{
    mListView = new ListView(this);
    setCentralWidget(mListView);

    mListModel = new ListModel;
    mListView->setModel(mListModel);

    mListItemDelegate = new ListItemDelegate(this);
    mListView->setItemDelegate(mListItemDelegate);
}


MainWindow::~MainWindow()
{
    
}
