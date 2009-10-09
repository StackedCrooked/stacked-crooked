#include "Test/ImageViewerSample.h"
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
    ImageViewerSample::ImageViewerSample() :
        mNativeWindow(0)
    {
    }


    void ImageViewerSample::run()
    {
        std::string chdir = "../xulrunnersamples/imageviewer/";
        CurrentDirectoryChanger curdir(chdir);

        //system("run.bat");

        XULRunner runner;
        mRootElement = runner.loadXUL("chrome://imageviewer/content/imageviewer.xul");
        if (!mRootElement)
        {
            return;
        }

        ScopedEventListener events;
        events.connect(mRootElement.get(), WM_DROPFILES, boost::bind(&ImageViewerSample::dropFiles, this, _1, _2));
       
        if (mNativeWindow = mRootElement->impl()->downcast<NativeWindow>())
        {
            ::DragAcceptFiles(mNativeWindow->handle(), TRUE);
            mNativeWindow->showModal();
        }
    }


    LRESULT ImageViewerSample::dropFiles(WPARAM wParam, LPARAM lParam)
    {
        Element * imageArea = mRootElement->getElementById("imagearea");
        if (!imageArea)
        {
            return 1;
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
        mNativeWindow->rebuildLayout();
        ::RedrawWindow(mNativeWindow->handle(), NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
        return 0;
    }


} // namespace XULWin
