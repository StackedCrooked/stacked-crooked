#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtGui/QMainWindow>
#include "GstSupport/GstSupport.h"
#include "GstSupport/GstWidget.h"
#include <boost/scoped_ptr.hpp>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    ~MainWindow();

private:
    Gst::ScopedObject<GstElement> mPipeline;
    boost::scoped_ptr<Gst::Widget> mGstWidget;
};


#endif // MAINWINDOW_H
