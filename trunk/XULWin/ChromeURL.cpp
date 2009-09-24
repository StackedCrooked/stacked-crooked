#include "ChromeURL.h"        


namespace XULWin
{

    ChromeURL::ChromeURL(const std::string & inURL, const std::string & inLocale) :
        mURL(inURL),
        mLocale(inLocale)
    {
    }


    const std::string & ChromeURL::stringValue()
    {
        return mURL;
    }


    std::string ChromeURL::convertToLocalPath()
    {
        static const std::string cChrome = "chrome://";
        static const std::string cLocale = "locale";

        // Change this pattern: chrome://myapp/skin/icons/myimg.jpg
        // Into this pattern:   chrome/skin/icons/myimg.jpg
        std::string result;
        result = mURL.substr(cChrome.size(), mURL.size() - cChrome.size());
        size_t slashIdx = result.find("/");
        if (slashIdx != std::string::npos)
        {
            result = "chrome/" + result.substr(slashIdx + 1, result.size() - slashIdx - 1);
        }

        size_t localeIdx = result.find(cLocale);
        if (localeIdx != std::string::npos)
        {
            result.insert(localeIdx + cLocale.size() + 1, mLocale + "/");
        }
        return result;
    }

    
} // XULWin
