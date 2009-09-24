#ifndef INITIALIZER_H_INCLUDED
#define INITIALIZER_H_INCLUDED


#include "ElementFactory.h"
#include <boost/noncopyable.hpp>
#include <windows.h>


namespace XULWin
{

    /**
     * Takes care of all XULWin library initializations and finalizations.
     */
    class Initializer : boost::noncopyable
    {
    public:
        Initializer(HINSTANCE inModuleHandle);

        ~Initializer();
    };


} // namespace XULWin

#endif // INITIALIZER_H_INCLUDED
