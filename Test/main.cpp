#include "Tester.h"
#include "XULWin/Decorator.h"
#include "XULWin/ElementImpl.h"
#include "XULWin/Initializer.h"
#include "XULWin/XULRunner.h"
#include "Utils/ErrorReporter.h"
#include "Utils/Fallible.h"
#include "Utils/WinUtils.h"
#include <windows.h>
#include <commctrl.h>


using namespace XULWin;


void log(const std::string & inMessage)
{
    //MessageBoxA(0, inMessage.c_str(), "XULWin Logger", MB_OK);
}


class TestConfigSample : public XULWin::EventListener
{
public:
    void run()
    {
        Utils::CurrentDirectoryChanger curdir("../xulrunnersamples/configpanel/");
        mConfigWindow = mRunner.loadApplication("application.ini");

        mNewSetButton = mConfigWindow->getElementById("newSetButton");
        //mNewSetButton->setClickAction(boost::bind(&TestConfigSample::showNewSetDialog, this));

        mSetsPopup = mConfigWindow->getElementById("setsPopup");

        if (NativeWindow * win = mConfigWindow->impl()->downcast<NativeWindow>())
        {        
            win->showModal();
        }
    }

    void addNewSet(const std::string & inSetName)
    { 
        AttributesMapping attr;
        attr["label"] = inSetName;
        ElementPtr item = MenuItem::Create(mSetsPopup, attr);
        item->init();
    }

    void showNewSetDialog()
    {
        mNewSetDlg = mRunner.loadXUL("chrome://configpanel/content/newsetdialog.xul");

        mNewSetTextBox = mNewSetDlg->getElementById("settextbox");
        
        mNewSetOK = mNewSetDlg->getElementById("newSetOKButton");
        //mNewSetOK->setClickAction(boost::bind(&TestConfigSample::newSetOK, this));

        mNewSetCancel = mNewSetDlg->getElementById("newSetCancelButton");
        //mNewSetCancel->setClickAction(boost::bind(&TestConfigSample::closeWindow, this, mNewSetDlg.get()));

        mNewSetDlg->impl()->downcast<NativeWindow>()->showModal();
    }

    void newSetOK()
    {        
        AttributesMapping attr;
        if (NativeTextBox * nativeTextBox = mNewSetTextBox->impl()->downcast<NativeTextBox>())
        {
            addNewSet(nativeTextBox->getValue());
        }
        closeWindow(mNewSetDlg.get());
    }

    void closeWindow(Element * inWindow)
    {
        if (NativeWindow * nativeWindow = inWindow->impl()->downcast<NativeWindow>())
        {
            ShowWindow(nativeWindow->handle(), SW_HIDE);
		    PostQuitMessage(0);
        }
    }
    
    virtual void handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
    }

    virtual void handleCommand(Element * inSender, unsigned short inNotificationCode)
    {
        //if (inSender->impl()->commandId() == mNewSetButton->impl()->commandId())
        //{
        //    showNewSetDialog();
        //}
        //else if (mNewSetOK && mNewSetOK->impl()->commandId() == inSender->impl()->commandId())
        //{
        //    newSetOK();
        //    closeWindow(mNewSetDlg.get());
        //}
        //else if (mNewSetCancel && mNewSetCancel->impl()->commandId() == inSender->impl()->commandId())
        //{
        //    closeWindow(mNewSetDlg.get());
        //}
    }

private:
    XULRunner mRunner;   
    ElementPtr mConfigWindow;
    ElementPtr mNewSetDlg;
    Utils::Fallible<Element*> mNewSetButton;
    Utils::Fallible<Element*> mSetsPopup;    
    Utils::Fallible<Element*> mNewSetTextBox;
    Utils::Fallible<Element*> mNewSetOK;
    Utils::Fallible<Element*> mNewSetCancel;
};


void runConfigSample()
{
    TestConfigSample test;
    test.run();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize all kinds of stuff
    XULWin::Initializer initializer(hInstance);

    // Ensure that the common control DLL is loaded. 
    Utils::CommonControlsInitializer ccInit;

    Utils::ErrorReporter::Instance().setLogger(boost::bind(&log, _1));

    //runConfigSample();
    XULTest::Tester tester;
    //tester.runXULSample("hello");
    tester.runXULSample("tabbox");
    //tester.runXULSample("widgets");
    //tester.runXULSample("configpanel");
    //tester.runXULSample("shout");
    return 0;
}
