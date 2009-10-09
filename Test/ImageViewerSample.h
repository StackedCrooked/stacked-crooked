#ifndef IMAGEVIEWER_H_INCLUDED
#define IMAGEVIEWER_H_INCLUDED


#include "XULWin/EventListener.h"
#include <string>
#include <windows.h>


namespace XULWin
{
    class NativeWindow;
    class ImageViewerSample
    {
    public:
        ImageViewerSample();

        void run();

        LRESULT dropFiles(WPARAM wParam, LPARAM lParam);

    private:
        ElementPtr mRootElement;
        NativeWindow * mNativeWindow;
        ScopedEventListener mEvents;
    };


} // namespace XULWin


#endif // IMAGEVIEWER_H_INCLUDED
