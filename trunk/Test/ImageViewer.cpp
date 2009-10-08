#include "Test/ImageViewer.h"
#include "XULWin/XULRunner.h"
#include "XULWin/EventListener.h"
#include "XULWin/Decorator.h"
#include "XULWin/ElementImpl.h"
#include "XULWin/Image.h"
#include "Utils/WinUtils.h"
#include <boost/bind.hpp>


using namespace Utils;


namespace XULWin
{
    ImageViewer::ImageViewer() :
        mNativeWindow(0)
    {
    }


    void ImageViewer::run()
    {
        std::string chdir = "../xulrunnersamples/imageviewer/";
        CurrentDirectoryChanger curdir(chdir);

        //system("run.bat");

        XULRunner runner;
        mRootElement = runner.loadXUL("chrome://imageviewer/content/imageviewer.xul");
        
        Window * win = mRootElement->downcast<Window>();
        if (!win)
        {
            return;
        }

        ScopedEventListener events;
        events.connect(win, WM_DROPFILES, boost::bind(&ImageViewer::dropFiles, this, _1, _2));

       
        if (mNativeWindow = win->impl()->downcast<NativeWindow>())
        {
            ::DragAcceptFiles(mNativeWindow->handle(), TRUE);
            mNativeWindow->showModal();
        }
    }


    void ImageViewer::dropFiles(WPARAM wParam, LPARAM lParam)
    {
        Element * imageArea = mRootElement->getElementById("imagearea");
        if (!imageArea)
        {
            return;
        }

        int numFiles = ::DragQueryFile((HDROP)wParam, 0xFFFFFFFF, 0, 0);
        for (int idx = 0; idx < numFiles; ++idx)
        {
            // Extract file name
        	TCHAR fileName[MAX_PATH];
            ::DragQueryFile((HDROP)wParam, idx, &fileName[0], MAX_PATH);

            // Create the image element
            AttributesMapping attr;
            attr["src"] = Utils::ToUTF8(&fileName[0]);
            attr["flex"] = "1";
            attr["width"] = "160";
            attr["flex"] = "0";
            attr["keepaspectratio"] = "1";
            ElementPtr image = Image::Create(imageArea, attr);
            image->init();
        }
    }


} // namespace XULWin
