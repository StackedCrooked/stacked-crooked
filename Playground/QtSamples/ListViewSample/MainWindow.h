#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "CentralWidget.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    CentralWidget * mCentralWidget;
};


#endif // MAINWINDOW_H
