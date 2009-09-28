#include "Conversions.h"
#include "Utils/ErrorReporter.h"
#include "Poco/String.h"
#include <boost/lexical_cast.hpp>


using namespace Utils;


namespace XULWin
{

    template<class T>
    std::string ToString(const T & inValue)
    {
        try
        {
            return boost::lexical_cast<std::string>(inValue);
        }
        catch (boost::bad_lexical_cast &)
        {
            ReportError("String2Int: lexical cast failed");
            return "";
        }
    }


    std::string Int2String(int inValue)
    {
        return ToString(inValue);
    }


    int String2Int(const std::string & inValue, int inDefault)
    {
        int result = inDefault;
        try
        {
            if (!inValue.empty())
            {
                result = boost::lexical_cast<int>(inValue);
            }
        }
        catch (std::exception &)
        {
            // ok, too bad
        }
        return result;
    }


    int String2Int(const std::string & inValue)
    {
        int result = 0;
        try
        {
            if (!inValue.empty())
            {
                result = boost::lexical_cast<int>(inValue);
            }
        }
        catch (std::exception &)
        {
            ReportError("Int2String: lexical cast failed");
        }
        return result;
    }
    
    
    bool String2Bool(const std::string & inString, bool inDefault)
    {
        if (inString == "true")
        {
            return true;
        }
        else if (inString == "false")
        {
            return false;
        }
        else
        {
            return inDefault;
        }
    }
    
    
    std::string Bool2String(bool inBool)
    {
        return inBool ? "true" : "false";
    }


    Orient String2Orient(const std::string & inValue, Orient inDefault)
    {
        Orient result = inDefault;
        if (inValue == "horizontal")
        {
            result = HORIZONTAL;
        }
        else if (inValue == "vertical")
        {
            result = VERTICAL;
        }
        return result;
    }


    std::string Orient2String(Orient inOrient)
    {
        if (inOrient == HORIZONTAL)
        {
            return "horizontal";
        }
        else
        {
            return "vertical";
        }
    }

    
    Align String2Align(const std::string & inValue, Align inDefault)
    {
        Align result = inDefault;
        if (inValue == "start")
        {
            result = Start;
        }
        else if (inValue == "center")
        {
            result = Center;
        }
        else if (inValue == "end")
        {
            result = End;
        }
        else if (inValue == "stretch")
        {
            result = Stretch;
        }
        return result;
    }


    std::string Align2String(Align inAlign)
    {
        if (inAlign == Start)
        {
            return "start";
        }
        else if (inAlign == Center)
        {
            return "center";
        }
        else if (inAlign == End)
        {
            return "end";
        }
        else if (inAlign == Stretch)
        {
            return "stretch";
        }
        else
        {
            ReportError("Invalid alignment");
            return "";
        }
    }
    
    
    int CssString2Size(const std::string & inString, int inDefault)
    {
        if (inString.rfind("px") != std::string::npos)
        {
            return String2Int(Poco::replace(inString, "px", ""), inDefault);
        }
        return String2Int(inString, inDefault);
    }

} // namespace XULWin
