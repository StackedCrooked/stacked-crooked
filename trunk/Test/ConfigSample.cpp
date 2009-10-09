#include "Test/ConfigSample.h"
#include <sstream>


namespace XULWin
{

    void ConfigSample::run()
    {
        Utils::CurrentDirectoryChanger curdir("../xulrunnersamples/configpanel/");
        mConfigWindow = mRunner.loadApplication("application.ini");

        mNewSetButton = mConfigWindow->getElementById("newSetButton");
        mEvents.connect(mNewSetButton, boost::bind(&ConfigSample::showNewSetDialog, this));
        
        Element * allowRatingsCheckbox = mConfigWindow->getElementById("allowRatingsCheckBox");
        mEvents.connect(allowRatingsCheckbox, boost::bind(&ConfigSample::showMessage, this, "Checked"));
        
        mSetsPopup = mConfigWindow->getElementById("setsPopup");
        
        Element * tagsText = mConfigWindow->getElementById("tagsTextBox");
        mEvents.connect(tagsText, WM_KEYUP, boost::bind(&ConfigSample::showMessage, this, "WM_KEYUP"));

        Element * uploadButton = mConfigWindow->getElementById("uploadButton");
        mEvents.connect(uploadButton, boost::bind(&ConfigSample::showUpload, this));

        if (NativeWindow * win = mConfigWindow->impl()->downcast<NativeWindow>())
        {
            ::DragAcceptFiles(win->handle(), TRUE);
            mEvents.connect(mConfigWindow.get(),
                            WM_DROPFILES,
                            boost::bind(&ConfigSample::dropFiles, this, _1, _2));

            Element * cancelButton = mConfigWindow->getElementById("cancelButton");
            mEvents.connect(cancelButton, boost::bind(&NativeWindow::endModal, win));
            win->showModal();
        }
    }


    LRESULT ConfigSample::dropFiles(WPARAM wParam, LPARAM lParam)
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


    LRESULT ConfigSample::showMessage(const std::string & inMessage)
    {
        std::stringstream ss;
        ss << inMessage;
        ::MessageBoxA(0, ss.str().c_str(), "Message", MB_OK);
        return 0;
    }


    LRESULT ConfigSample::showUpload()
    {
        ::MessageBox(0, TEXT("Upload initiated!"), TEXT("Config panel"), MB_OK);
        return 0;
    }


    LRESULT ConfigSample::addNewSet(const std::string & inSetName)
    { 
        AttributesMapping attr;
        attr["label"] = inSetName;
        ElementPtr item = MenuItem::Create(mSetsPopup, attr);
        item->init();
        return 0;
    }


    LRESULT ConfigSample::showNewSetDialog()
    {
        mNewSetDlg = mRunner.loadXUL("chrome://configpanel/content/newsetdialog.xul");

        mNewSetTextBox = mNewSetDlg->getElementById("settextbox");
        
        mNewSetOK = mNewSetDlg->getElementById("newSetOKButton");
        ScopedEventListener localEvents;
        localEvents.connect(mNewSetOK, boost::bind(&ConfigSample::newSetOK, this));

        mNewSetCancel = mNewSetDlg->getElementById("newSetCancelButton");
        localEvents.connect(mNewSetCancel, boost::bind(&ConfigSample::closeWindow, this, mNewSetDlg.get()));

        mNewSetDlg->impl()->downcast<NativeWindow>()->showModal();
        return 0;
    }


    LRESULT ConfigSample::newSetOK()
    {        
        AttributesMapping attr;
        if (NativeTextBox * nativeTextBox = mNewSetTextBox->impl()->downcast<NativeTextBox>())
        {
            addNewSet(nativeTextBox->getValue());
        }
        closeWindow(mNewSetDlg.get());
        return 0;
    }


    LRESULT ConfigSample::closeWindow(Element * inWindow)
    {
        if (NativeWindow * nativeWindow = inWindow->impl()->downcast<NativeWindow>())
        {
            ShowWindow(nativeWindow->handle(), SW_HIDE);
	        PostQuitMessage(0);
            return 0;
        }
        return 1;
    }


} // namespace XULWin
