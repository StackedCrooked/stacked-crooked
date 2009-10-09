#ifndef CONFIGSAMPLE_H_INCLUDED
#define CONFIGSAMPLE_H_INCLUDED


#include "ImageViewerSample.h"
#include "XULWin/Decorator.h"
#include "XULWin/EventListener.h"
#include "XULWin/ElementImpl.h"
#include "XULWin/Initializer.h"
#include "XULWin/XULRunner.h"
#include "Utils/ErrorReporter.h"
#include "Utils/Fallible.h"
#include "Utils/WinUtils.h"
#include <string>
#include <windows.h>


namespace XULWin
{
    class Element;

    class ConfigSample
    {
    public:
        void run();

        LRESULT dropFiles(WPARAM wParam, LPARAM lParam);

        LRESULT showMessage(const std::string & inMessage);

        LRESULT showUpload();

        LRESULT addNewSet(const std::string & inSetName);

        LRESULT showNewSetDialog();

        LRESULT newSetOK();

        LRESULT closeWindow(Element * inWindow);

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


} // namespace XULWin


#endif // CONFIGSAMPLE_H_INCLUDED
