#include "Graphics.h"
#include <assert.h>
#include <gdiplus.h>


namespace XULWin
{    

    int GdiplusLoader::sRefCount;


    GdiplusLoader::GdiplusLoader()
    {
        assert(sRefCount >= 0);
        if (sRefCount++ == 0)
        {
            // Init Gdiplus
            Gdiplus::GdiplusStartupInput gdiplusStartupInput;
            Gdiplus::GdiplusStartup(&mGdiPlusToken, &gdiplusStartupInput, NULL);
        }
    }


    GdiplusLoader::~GdiplusLoader()
    {
        assert(sRefCount >= 0);
        if (--sRefCount == 0)
        {
            Gdiplus::GdiplusShutdown(mGdiPlusToken);
        }
        assert(sRefCount >= 0);
    }

} // namespace XULWin
