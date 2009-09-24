#ifndef XULRUNNER_H_INCLUDED
#define XULRUNNER_H_INCLUDED


#include <string>


namespace XULWin
{
    
    class XULRunner
    {
    public:
        /**
         * Params:
         * inApplicationIniFile: path to ini file
         */
        void run(const std::string & inApplicationIniFile);
    
    };

} // XULWin


#endif // XULRUNNER_H_INCLUDED
