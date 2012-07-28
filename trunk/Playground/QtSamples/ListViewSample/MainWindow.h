#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "CentralWidget.h"


class FileMenu : public QMenu
{
    Q_OBJECT

public:
    struct EventHandler
    {
        virtual void onFileMenuTriggered(FileMenu&) = 0;
    };

    FileMenu(QMenuBar * inMenuBar, const QString & inTitle, EventHandler & inEventHandler) :
        QMenu(inMenuBar),
        mEventHandler(inEventHandler)
    {
        QAction * action = addAction("&Open...");
        action->setShortcut(QKeySequence("Ctrl+O"));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(onOpen()));
        setTitle(inTitle);
        inMenuBar->addMenu(this);
    }

private Q_SLOTS:
    void onOpen()
    {
        mEventHandler.onFileMenuTriggered(*this);
    }

private:
    EventHandler & mEventHandler;
};


class MainWindow : public QMainWindow,
                   FileMenu::EventHandler
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void onFileMenuTriggered(FileMenu&);

    CentralWidget * mCentralWidget;
};


#endif // MAINWINDOW_H
