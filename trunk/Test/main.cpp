#include "Tester.h"
#include "XULWin/Initializer.h"
#include "Utils/ErrorReporter.h"
#include "Utils/WinUtils.h"
#include <windows.h>
#include <commctrl.h>


void log(const std::string & inMessage)
{
    //MessageBoxA(0, inMessage.c_str(), "XULWin Logger", MB_OK);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize all kinds of stuff
    XULWin::Initializer initializer(hInstance);

    // Ensure that the common control DLL is loaded. 
    Utils::CommonControlsInitializer ccInit;

    Utils::ErrorReporter::Instance().setLogger(boost::bind(&log, _1));

    XULTest::Tester tester;
    //tester.runXULSample("hello");
    //tester.runXULSample("uploadr");
    tester.runXULSample("configpanel");
    return 0;
}
