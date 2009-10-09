#include "Tester.h"
#include "ConfigSample.h"
#include <boost/bind.hpp>
#include <sstream>
#include <windows.h>
#include <commctrl.h>


using namespace XULWin;


void log(const std::string & inMessage)
{
    //MessageBoxA(0, inMessage.c_str(), "XULWin Logger", MB_OK);
}


void runConfigSample()
{
    ConfigSample test;
    test.run();
}


void runImageViewerSample()
{
    ImageViewerSample sample;
    sample.run();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize all kinds of stuff
    XULWin::Initializer initializer(hInstance);

    // Ensure that the common control DLL is loaded. 
    Utils::CommonControlsInitializer ccInit;

    Utils::ErrorReporter::Instance().setLogger(boost::bind(&log, _1));

    //runConfigSample();
    runImageViewerSample();
    //XULWin::Tester tester;
    //tester.runXULSample("hello");
    //tester.runXULSample("toolbar");
    //tester.runXULSample("widgets");
    //tester.runXULSample("tabbox");
    //tester.runXULSample("treeview");
    //tester.runXULSample("configpanel");
    //tester.runXULSample("shout");
    //tester.runXULSample("svg");
    return 0;
}
