#ifndef XULRUNNER_H_INCLUDED
#define XULRUNNER_H_INCLUDED


#include "Parser.h"
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

        ElementPtr loadApplication(const std::string & inApplicationIniFile);

        ElementPtr loadXUL(const std::string & inXULUrl);

    private:
        Parser mParser;
    
    };

} // XULWin


#endif // XULRUNNER_H_INCLUDED
