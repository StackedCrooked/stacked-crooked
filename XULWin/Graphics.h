#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED


#include <windows.h>
#include <boost/noncopyable.hpp>


namespace XULWin
{

    class GdiplusLoader : boost::noncopyable
    {
        public:
            GdiplusLoader();

            ~GdiplusLoader();

        private:
            ULONG_PTR mGdiPlusToken;
            static int sRefCount;
    };


} // namespace XULWin

#endif // GRAPHICS_H_INCLUDED
