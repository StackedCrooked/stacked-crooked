#include "Tester.h"
#include "ImageViewer.h"
#include "XULWin/Decorator.h"
#include "XULWin/EventListener.h"
#include "XULWin/ElementImpl.h"
#include "XULWin/Initializer.h"
#include "XULWin/XULRunner.h"
#include "Utils/ErrorReporter.h"
#include "Utils/Fallible.h"
#include "Utils/WinUtils.h"
#include <boost/bind.hpp>
#include <sstream>
#include <windows.h>
#include <commctrl.h>


using namespace XULWin;


void log(const std::string & inMessage)
{
    //MessageBoxA(0, inMessage.c_str(), "XULWin Logger", MB_OK);
}


class TestConfigSample
{
public:
    void run()
    {
        Utils::CurrentDirectoryChanger curdir("../xulrunnersamples/configpanel/");
        mConfigWindow = mRunner.loadApplication("application.ini");

        mNewSetButton = mConfigWindow->getElementById("newSetButton");
        mEvents.connect(mNewSetButton, boost::bind(&TestConfigSample::showNewSetDialog, this));
        
        Element * allowRatingsCheckbox = mConfigWindow->getElementById("allowRatingsCheckBox");
        mEvents.connect(allowRatingsCheckbox, boost::bind(&TestConfigSample::showMessage, this, "Checked"));
        
        mSetsPopup = mConfigWindow->getElementById("setsPopup");
        
        Element * tagsText = mConfigWindow->getElementById("tagsTextBox");
        mEvents.connect(tagsText, WM_KEYUP, boost::bind(&TestConfigSample::showMessage, this, "WM_KEYUP"));

        Element * uploadButton = mConfigWindow->getElementById("uploadButton");
        mEvents.connect(uploadButton, boost::bind(&TestConfigSample::showUpload, this));

        if (NativeWindow * win = mConfigWindow->impl()->downcast<NativeWindow>())
        {
            ::DragAcceptFiles(win->handle(), TRUE);
            mEvents.connect(mConfigWindow.get(),
                            WM_DROPFILES,
                            boost::bind(&TestConfigSample::dropFiles, this, _1, _2));

            Element * cancelButton = mConfigWindow->getElementById("cancelButton");
            mEvents.connect(cancelButton, boost::bind(&NativeWindow::endModal, win));
            win->showModal();
        }
    }

    LRESULT dropFiles(WPARAM wParam, LPARAM lParam)
    {
        std::vector<std::string> files;
        int numFiles = ::DragQueryFile((HDROP)wParam, 0xFFFFFFFF, 0, 0);
        for (int idx = 0; idx < numFiles; ++idx)
        {
        	TCHAR fileName[MAX_PATH];
            ::DragQueryFile((HDROP)wParam, idx, &fileName[0], MAX_PATH);
            files.push_back(Utils::ToUTF8(&fileName[0]));
        }
        return 0;
    }

    LRESULT showMessage(const std::string & inMessage)
    {
        std::stringstream ss;
        ss << inMessage;
        ::MessageBoxA(0, ss.str().c_str(), "Message", MB_OK);
        return 0;
    }

    LRESULT showUpload()
    {
        ::MessageBox(0, TEXT("Upload initiated!"), TEXT("Config panel"), MB_OK);
        return 0;
    }

    LRESULT addNewSet(const std::string & inSetName)
    { 
        AttributesMapping attr;
        attr["label"] = inSetName;
        ElementPtr item = MenuItem::Create(mSetsPopup, attr);
        item->init();
        return 0;
    }

    LRESULT showNewSetDialog()
    {
        mNewSetDlg = mRunner.loadXUL("chrome://configpanel/content/newsetdialog.xul");

        mNewSetTextBox = mNewSetDlg->getElementById("settextbox");
        
        mNewSetOK = mNewSetDlg->getElementById("newSetOKButton");
        ScopedEventListener localEvents;
        localEvents.connect(mNewSetOK, boost::bind(&TestConfigSample::newSetOK, this));

        mNewSetCancel = mNewSetDlg->getElementById("newSetCancelButton");
        localEvents.connect(mNewSetCancel, boost::bind(&TestConfigSample::closeWindow, this, mNewSetDlg.get()));

        mNewSetDlg->impl()->downcast<NativeWindow>()->showModal();
        return 0;
    }

    LRESULT newSetOK()
    {        
        AttributesMapping attr;
        if (NativeTextBox * nativeTextBox = mNewSetTextBox->impl()->downcast<NativeTextBox>())
        {
            addNewSet(nativeTextBox->getValue());
        }
        closeWindow(mNewSetDlg.get());
        return 0;
    }

    LRESULT closeWindow(Element * inWindow)
    {
        if (NativeWindow * nativeWindow = inWindow->impl()->downcast<NativeWindow>())
        {
            ShowWindow(nativeWindow->handle(), SW_HIDE);
		    PostQuitMessage(0);
            return 0;
        }
        return 1;
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
    ScopedEventListener mEvents;
};


void runConfigSample()
{
    TestConfigSample test;
    test.run();
}


void runImageViewerSample()
{
    ImageViewer sample;
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
    //runImageViewerSample();
    XULTest::Tester tester;
    //tester.runXULSample("hello");
    tester.runXULSample("netlog-toolbar");
    //tester.runXULSample("widgets");
    //tester.runXULSample("tabbox");
    //tester.runXULSample("treeview");
    //tester.runXULSample("configpanel");
    //tester.runXULSample("shout");
    //tester.runXULSample("svg");
    return 0;
}
