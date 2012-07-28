#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "CentralWidget.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>


class Menu;


class MenuItem : public QObject
{
    Q_OBJECT

public:
    MenuItem(Menu * inMenu, const QString & inText, int inIndex) :
        QObject(),
        mMenu(inMenu),
        mText(inText),
        mIndex(inIndex),
        mAction(),
        mCallback()
    {
    }

    int getIndex() const
    {
        return mIndex;
    }

    QString getText() const
    {
        return mText;
    }

    const Menu & getMenu() const
    {
        return *mMenu;
    }

    const QAction * getAction() const
    {
        return mAction;
    }

    typedef boost::function<void(MenuItem&)> Callback;

    void connectAction(QAction * inAction, const Callback & inCallback)
    {
        this->connect(inAction, SIGNAL(triggered(bool)), this, SLOT(onMenuTriggered()));
        mAction = inAction;
        mCallback = inCallback;
    }

private Q_SLOTS:
    void onMenuTriggered()
    {
        mCallback(*this);
    }

private:
    Menu * mMenu;    
    QString mText;
    int mIndex;
    QAction * mAction;
    Callback mCallback;
};


class Menu : public QMenu
{
public:
    struct EventHandler
    {
        virtual void onMenuTriggered(Menu&, MenuItem&) = 0;
    };

    Menu(QMenuBar * inMenuBar, const QString & inTitle, EventHandler & inEventHandler) :
        QMenu(inMenuBar),
        mEventHandler(inEventHandler)
    {
        setTitle(inTitle);
        inMenuBar->addMenu(this);
    }

    MenuItem * addMenuItem(const QString & inText, const QKeySequence & inShortCut)
    {
        QAction * action = addAction(inText);
        action->setShortcut(inShortCut);

        MenuItem * theMenuItem = new MenuItem(this, inText, mMenuItems.size());
        theMenuItem->connectAction(action, boost::bind(&Menu::onOpen, this, boost::ref(*theMenuItem)));
        return theMenuItem;
    }

private:
    void onOpen(MenuItem & inMenuItem)
    {
        mEventHandler.onMenuTriggered(*this, inMenuItem);
    }

    EventHandler & mEventHandler;
    QList<MenuItem*> mMenuItems;
};


class MainWindow : public QMainWindow,
                   Menu::EventHandler
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void onMenuTriggered(Menu&, MenuItem&);

    CentralWidget * mCentralWidget;
    MenuItem * mFileOpenEvent;
};


#endif // MAINWINDOW_H
