#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "CentralWidget.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>


class Menu;


class MenuEvent : public QObject
{
    Q_OBJECT

public:
    MenuEvent(Menu * inMenu, const QString & inText, int inIndex) :
        QObject(),
        mMenu(inMenu),
        mText(inText),
        mIndex(inIndex)
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

    typedef boost::function<void(MenuEvent&)> Callback;

    void connectAction(QAction * inAction, const Callback & inCallback)
    {
        this->connect(inAction, SIGNAL(triggered(bool)), this, SLOT(onMenuTriggered()));
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
    Callback mCallback;
};


class Menu : public QMenu
{
public:
    struct EventHandler
    {
        virtual void onMenuTriggered(Menu&, MenuEvent&) = 0;
    };

    Menu(QMenuBar * inMenuBar, const QString & inTitle, EventHandler & inEventHandler) :
        QMenu(inMenuBar),
        mEventHandler(inEventHandler)
    {
        setTitle(inTitle);
        inMenuBar->addMenu(this);
    }

    MenuEvent * addMenuItem(const QString & inText, const QKeySequence & inShortCut)
    {
        QAction * action = addAction(inText);
        action->setShortcut(inShortCut);

        MenuEvent * theMenuEvent = new MenuEvent(this, inText, mMenuEvents.size());
        theMenuEvent->connectAction(action, boost::bind(&Menu::onOpen, this, boost::ref(*theMenuEvent)));
        return theMenuEvent;
    }

private:
    void onOpen(MenuEvent & inMenuEvent)
    {
        mEventHandler.onMenuTriggered(*this, inMenuEvent);
    }

    EventHandler & mEventHandler;
    QList<MenuEvent*> mMenuEvents;
};


class MainWindow : public QMainWindow,
                   Menu::EventHandler
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void onMenuTriggered(Menu&, MenuEvent&);

    CentralWidget * mCentralWidget;
    MenuEvent * mFileOpenEvent;
};


#endif // MAINWINDOW_H
