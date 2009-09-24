#include "XULRunner.h"
#include "ChromeURL.h"
#include "Defaults.h"
#include "Utils/ErrorReporter.h"
#include "Parser.h"
#include "Poco/Path.h"
#include "Poco/String.h"


using namespace Utils;


namespace XULWin
{

    typedef std::map<std::string, std::string> Prefs;


    bool parsePrefsLine(const std::string & inPrefsLine, std::pair<std::string, std::string> & outPref)
    {
        try
        {
            //pref("toolkit.defaultChromeURI", "chrome://myapp/content/myapp.xul");
            std::string::size_type begin = inPrefsLine.find_first_of("\"") + 1;
            std::string::size_type end = inPrefsLine.find_first_of("\"", begin);
            outPref.first = inPrefsLine.substr(begin, end - begin);
            
            end++;
            begin = inPrefsLine.find_first_of("\"", end) + 1;
            end = inPrefsLine.find_first_of("\"", begin);
            outPref.second = inPrefsLine.substr(begin, end - begin);
        }
        catch (const std::exception & inExc)
        {
            ReportError(inExc.what());
            return false;
        }
        return true;
    }


    bool getPrefs(const std::string & inPrefsFile, Prefs & outPrefs)
    {
        
        static char str[1024];
		FILE *fp;
		fp = fopen(inPrefsFile.c_str(), "r");
		if(!fp)
		{
            ReportError("Failed to open prefs file: " + inPrefsFile);
			return false;
		}
		
		while (fgets(str, sizeof(str), fp) != NULL)
		{
			// strip trailing '\n' if it exists
			int len = strlen(str)-1;
			if(str[len] == '\n')
			{
				str[len] = 0;
			}
            std::pair<std::string, std::string> pair;
            if (parsePrefsLine(str, pair))
            {
                outPrefs.insert(pair);
            }
            else
            {
                ReportError("Could not parse pref: " + std::string(str));
            }
		}
		fclose(fp);
        return true;
    }

    
    std::string getMainXULFile(const Poco::Path & inTopLevelAppDir)
    {
        static const std::string cPrefsFile = "{appname}/defaults/preferences/prefs.js";
        std::string appName = inTopLevelAppDir.directory(inTopLevelAppDir.depth() - 1);
        std::string prefsFile = Poco::replace(cPrefsFile, std::string("{appname}"), appName);

        Prefs prefs;
        if (!getPrefs(prefsFile, prefs))
        {
            ReportError("Could not parse prefs file.");
            return "";
        }
        
        Prefs::iterator it = prefs.find("toolkit.defaultChromeURI");
        if (it != prefs.end())
        {
            // "chrome://myapp/content/myapp.xul"
            ChromeURL url(it->second, Defaults::locale());
            std::string xulFile = url.convertToLocalPath();
            return xulFile;
        }
        return "";
    }


    void XULRunner::run(const std::string & inApplicationIniFile)
    {       
	    Parser parser;
        Poco::Path topLevelAppDir = Poco::Path(inApplicationIniFile).parent();
        std::string mainXULFile = getMainXULFile(topLevelAppDir);
        parser.parse(mainXULFile);
        if (Window * window = parser.rootElement()->downcast<Window>())
        {
            window->showModal();
        }
    }

} // namespace XULWin
