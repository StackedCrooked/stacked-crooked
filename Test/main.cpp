#include "Tester.h"
#include "XULWin/Decorator.h"
#include "XULWin/ElementImpl.h"
#include "XULWin/Initializer.h"
#include "XULWin/XULRunner.h"
#include "Utils/ErrorReporter.h"
#include "Utils/WinUtils.h"
#include <windows.h>
#include <commctrl.h>


using namespace XULWin;


void log(const std::string & inMessage)
{
    //MessageBoxA(0, inMessage.c_str(), "XULWin Logger", MB_OK);
}


class TestConfigSample : public XULWin::EventHandler
{
public:
    XULWin::ElementPtr mRootEl;
    void run()
    {
        Utils::CurrentDirectoryChanger curdir("../xulrunnersamples/configpanel/");
        XULWin::XULRunner runner;
        mRootEl = runner.load("application.ini");

        Element * newSetButton = mRootEl->getElementById("newSetButton");
        newSetButton->addEventHandler(this);

        Element * setList = mRootEl->getElementById("setList");
        setList->addEventHandler(this);

        if (NativeWindow * win = mRootEl->impl()->downcast<NativeWindow>())
        {        
            win->showModal();
        }
    }
    
    virtual void handleMessage(Element * inSender, UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
    }

    virtual void command(Element * inSender, unsigned short inNotificationCode)
    {
        if (inSender->getAttribute("id") == "newSetButton")
        {
            MessageBox(0, TEXT("Command event from new set button"), 0, MB_OK);
        }
        else if (inSender->getAttribute("id") == "setList")
        {   
            int a = 0;
            a++;
        }
        else
        {
            MessageBox(0, TEXT("Command event from unknown sender"), 0, MB_OK);
        }
    }
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

    runConfigSample();
    //XULTest::Tester tester;
    //tester.runXULSample("hello");
    ////tester.runXULSample("uploadr");
    //tester.runXULSample("widgets");
    //tester.runXULSample("configpanel");
    //tester.runXULSample("shout");
    return 0;
}
