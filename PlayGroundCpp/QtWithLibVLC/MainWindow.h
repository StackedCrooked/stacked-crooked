#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "VLCPlayer.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    VLCPlayer * mDesktopVideo;
    VLCPlayer * mPresenterVideo;
};

#endif // MAINWINDOW_H
