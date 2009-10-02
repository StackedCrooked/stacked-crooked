#include "Conversions.h"
#include "Utils/ErrorReporter.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
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
            result = Horizontal;
        }
        else if (inValue == "vertical")
        {
            result = Vertical;
        }
        return result;
    }


    std::string Orient2String(Orient inOrient)
    {
        if (inOrient == Horizontal)
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


    int String2CSSSize(const std::string & inString, int inDefault)
    {
        if (inString.rfind("px") != std::string::npos)
        {
            return String2Int(Poco::replace(inString, "px", ""), inDefault);
        }
        return String2Int(inString, inDefault);
    }
    
    
    std::string CSSTextAlign2String(CSSTextAlign inTextAlign)
    {
        switch (inTextAlign)
        {
            case CSSTextAlign_Left: return "left";
            case CSSTextAlign_Center: return "center";  
            case CSSTextAlign_Right: return "right";
            //case CSSTextAlign_Justify: return "justify";
            default: return "left";
        }
    }


    CSSTextAlign String2CSSTextAlign(const std::string & inTextAlign, CSSTextAlign inDefault)
    {
        if (inTextAlign == "left") return CSSTextAlign_Left;
        else if (inTextAlign == "center") return CSSTextAlign_Center;
        else if (inTextAlign == "right") return CSSTextAlign_Right;
        //else if (inTextAlign == "justify") return CSSTextAlign_Justify;
        return CSSTextAlign_Left;
    }

    
    std::string CSSOverflow2String(CSSOverflow inTextAlign)
    {
        switch (inTextAlign)
        {
            case CSSOverflow_Visible: return "visible";
            case CSSOverflow_Hidden: return "hidden";  
            case CSSOverflow_Scroll: return "scroll";
            case CSSOverflow_Auto: return "auto";
            default: return "visible";
        }
    }


    CSSOverflow String2CSSOverflow(const std::string & inTextAlign, CSSOverflow inDefault)
    {
        if (inTextAlign == "visible") return CSSOverflow_Visible;
        else if (inTextAlign == "hidden") return CSSOverflow_Hidden;
        else if (inTextAlign == "scroll") return CSSOverflow_Scroll;
        else if (inTextAlign == "auto") return CSSOverflow_Auto;
        return CSSOverflow_Visible;
    }


    std::string RGBColor2String(const RGBColor & inColor)
    {
        return "blue";
    }


    RGBColor String2RGBColor(const std::string & inValue, const RGBColor & inDefault)
    {
        size_t offsetHexColor = inValue.find("#");
        if (offsetHexColor != std::string::npos)
        {
            std::string htmlColor = inValue.substr(offsetHexColor + 1, inValue.size() - offsetHexColor - 1);
            std::string colorComponent;
            std::vector<int> colors;
            for (size_t idx = 0; idx != htmlColor.size(); ++idx)
            {
                colorComponent += htmlColor[idx];
                if (idx%2 == 1)
                {
                    int hex = 0;
                    sscanf(colorComponent.c_str(), "%x", &hex);                    
                    colors.push_back(hex);
                    colorComponent.clear();
                }
            }
            if (colors.size() == 3)
            {
                return RGBColor(colors[0], colors[1], colors[2]);
            }
        }
        return RGBColor(0, 0, 255);
    }


    std::string Point2String(const Point & inPoint)
    {
        std::stringstream ss;
        ss << boost::lexical_cast<std::string>(inPoint.x());
        ss << ",";
        ss << boost::lexical_cast<std::string>(inPoint.y());
        return ss.str();
    }


    Point String2Point(const std::string & inValue)
    {
        int sep = inValue.find(",");
        if (sep != std::string::npos)
        {
            int x = boost::lexical_cast<int>(inValue.substr(0, sep));
            int y = boost::lexical_cast<int>(inValue.substr(sep + 1, inValue.size() - sep - 1));
            return Point(x, y);
        }
        return Point();
    }


    std::string Points2String(const Points & inPoint)
    {
        std::stringstream ss;
        Points::const_iterator it = inPoint.begin(), end = inPoint.end(); 
        for (; it != end; ++it)
        {
            ss << Point2String(*it) << " ";
        }
        return ss.str();
    }


    Points String2Points(const std::string & inValue)
    {
        Points result;
        Poco::StringTokenizer tokenizer(inValue, " ", Poco::StringTokenizer::TOK_IGNORE_EMPTY | Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = tokenizer.begin(), end = tokenizer.end();
        for (; it != end; ++it)
        {
            result.push_back(String2Point(*it));
        }
        return result;
    }

} // namespace XULWin
