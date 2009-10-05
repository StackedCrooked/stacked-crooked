#include "ChromeURL.h"
#include "Poco/String.h"


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
        if (mURL.empty() || mURL.size() < cChrome.size() || (mURL.find(cChrome) == std::string::npos))
        {
            return "";
        }

        // Change this pattern: chrome://myapp/skin/icons/myimg.jpg
        // Into this pattern:   chrome/skin/icons/myimg.jpg
        std::string result;
        
        // remove "chrome://"
        result = mURL.substr(cChrome.size(), mURL.size() - cChrome.size());

        // remove myapp
        size_t slashIdx = result.find("/");
        result = result.substr(slashIdx, result.size() - slashIdx);

        // prepend "chrome"
        result = "chrome" + result;

        // replace 'locale' with 'locale/en-US' (or whatever locale is selected)
        static const std::string cLocale = "locale";
        size_t localeIdx = result.find(cLocale);
        if (localeIdx != std::string::npos)
        {
            result.insert(localeIdx + cLocale.size() + 1, mLocale + "/");
        }
        return result;
    }

    
} // XULWin
