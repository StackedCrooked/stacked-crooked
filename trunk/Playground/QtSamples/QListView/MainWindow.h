#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "ListView.h"
#include "Importer.h"
#include <memory>


class ListView;
class ListModel;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget * inParent = 0);

private Q_SLOTS:
    void onTimerEvent();


private:
    ListView * mListView;
    ListModel * mListModel;
    QLabel * mLabel;
    QTimer * mTimer;
    std::unique_ptr<Mesmerize::Importer> mImporter;
};


#endif // MAINWINDOW_H
