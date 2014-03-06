#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtGui/QtGui>
#include "Importer.h"
#include <memory>


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    MainWindow(QWidget * inParent = 0);

private Q_SLOTS:
    void onTimerEvent();

private:
    void import(const std::string & inPath);

    QListWidget * mListWidget;
    std::unique_ptr<Mesmerize::Importer> mImporter;

};


#endif // MAINWINDOW_H
