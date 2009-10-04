#include "Conversions.h"
#include "Utils/ErrorReporter.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include <boost/lexical_cast.hpp>
#include <map>


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
    

    float String2Float(const std::string & inValue)
    {
        float result(0);
        try
        {
            if (!inValue.empty())
            {
                result = boost::lexical_cast<float>(inValue);
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


    bool HTMLColor2RGBColor(const std::string & inHTMLColor, RGBColor & outResult)
    {
        std::string colorComponent;
        std::vector<int> colors;
        for (size_t idx = 0; idx != inHTMLColor.size(); ++idx)
        {
            colorComponent += inHTMLColor[idx];
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
            outResult = RGBColor(colors[0], colors[1], colors[2]);
            return true;
        }
        return false;
    }

    
    bool RGBString2RGBColor(const std::string & inValue, RGBColor & outRGBColor)
    {
        size_t begin = inValue.find("(");
        if (begin == std::string::npos)
        {
            return false;
        }

        size_t end = inValue.find(")");
        if (end == std::string::npos)
        {
            return false;
        }
        
        Poco::StringTokenizer tok(inValue.substr(begin + 1, end - begin -1),
                                  ",",
                                  Poco::StringTokenizer::TOK_IGNORE_EMPTY
                                  | Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = tok.begin();
        std::vector<int> colors;
        for (; it != tok.end(); ++it)
        {
            int colorValue = 0;
            sscanf((*it).c_str(), "%d", &colorValue);                    
            colors.push_back(colorValue);
        }

        if (colors.size() == 3)
        {
            outRGBColor = RGBColor(colors[0], colors[1], colors[2]);
            return true;
        }
        return false;
    }


    bool String2RGBColor(const std::string & inValue, RGBColor & outResult)
    {
        size_t offset = inValue.find("#");
        if (offset != std::string::npos)
        {
            std::string htmlColor = inValue.substr(offset + 1, inValue.size() - offset - 1);
            if (HTMLColor2RGBColor(htmlColor, outResult))
            {
                return true;
            }
        }
        
        offset = inValue.find("rgb");
        if (offset != std::string::npos)
        {
            std::string rgbString = inValue.substr(offset, inValue.size() - offset);
            if (RGBString2RGBColor(rgbString, outResult))
            {
                return true;
            }
        }
        return false;
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

    
    std::string PathInstructions2String(const PathInstructions & inPathInstructions)
    {
        return ""; // TODO: implement
    }


    bool isInstruction(char c)
    {
        const static std::string fSeps = "MmLlHhVvCcSsQqTtAaZz";
        return fSeps.find(c) != std::string::npos;
    }


    bool isPathSeparator(char c)
    {
        return c == ',' || c == '-';
    }


    bool getInstructionType(char c,
                            PathInstruction::Type & outType,
                            PathInstruction::Positioning & outPositioning)
    {
        typedef std::map<char, std::pair<PathInstruction::Type, PathInstruction::Positioning> > Types;
        static Types fTypes;
        if (fTypes.empty())
        {            
            fTypes.insert(std::make_pair('M', std::make_pair(PathInstruction::MoveTo, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('m', std::make_pair(PathInstruction::MoveTo, PathInstruction::Relative)));
            fTypes.insert(std::make_pair('L', std::make_pair(PathInstruction::LineTo, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('l', std::make_pair(PathInstruction::LineTo, PathInstruction::Relative)));
            fTypes.insert(std::make_pair('H', std::make_pair(PathInstruction::HorizontalLineTo, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('h', std::make_pair(PathInstruction::HorizontalLineTo, PathInstruction::Relative)));
            fTypes.insert(std::make_pair('V', std::make_pair(PathInstruction::VerticalLineTo, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('v', std::make_pair(PathInstruction::VerticalLineTo, PathInstruction::Relative)));            
            fTypes.insert(std::make_pair('C', std::make_pair(PathInstruction::CurveTo, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('c', std::make_pair(PathInstruction::CurveTo, PathInstruction::Relative)));            
            fTypes.insert(std::make_pair('S', std::make_pair(PathInstruction::SmoothCurveTo, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('s', std::make_pair(PathInstruction::SmoothCurveTo, PathInstruction::Relative)));            
            fTypes.insert(std::make_pair('Q', std::make_pair(PathInstruction::QuadraticBelzierCurve, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('q', std::make_pair(PathInstruction::QuadraticBelzierCurve, PathInstruction::Relative)));            
            fTypes.insert(std::make_pair('T', std::make_pair(PathInstruction::SmoothQuadraticBelzierCurveTo, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('t', std::make_pair(PathInstruction::SmoothQuadraticBelzierCurveTo, PathInstruction::Relative)));            
            fTypes.insert(std::make_pair('A', std::make_pair(PathInstruction::EllipticalArc, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('a', std::make_pair(PathInstruction::EllipticalArc, PathInstruction::Relative)));            
            fTypes.insert(std::make_pair('Z', std::make_pair(PathInstruction::ClosePath, PathInstruction::Absolute)));
            fTypes.insert(std::make_pair('z', std::make_pair(PathInstruction::ClosePath, PathInstruction::Relative)));
        }
        Types::iterator it = fTypes.find(c);
        if (it != fTypes.end())
        {
            outType = it->second.first;
            outPositioning = it->second.second;
            return true;
        }
        return false;
    }


    void addPoint(bool & ioParsingX,
                  bool & ioParsingY,
                  std::string & ioX,
                  std::string & ioY,
                  PointFs & outPoints)
    {
        outPoints.push_back(PointF(String2Float(ioX), String2Float(ioY)));
        ioX.clear();
        ioY.clear();
        ioParsingX = true;
        ioParsingY = false;
    }


    PathInstructions String2PathInstructions(const std::string & inValue)
    {
        // "MmLlHhVvCcSsQqTtAaZz"
        // M205.2,17.1 c-33.6-9.7-42,19.1-48.2,22.6 c-6.2,3.5-27.9,2.2-33.3,5.8 c-5.3,3.5-17.3,23.5-8.4,41.6 c8.9,18.2,32.3,10.2,32.3,10.2 s-10.6,11.1-29.7,19.9 c-1,0.5-2.1,0.9-3.1,1.3 c-1.6,6.2-3,17.3,5,23.4 c0.4,0.4-10.8-0.9-13.9-3.1 c-2.6-1.9-4.9-9.8-5.6-15.9 c-10.9,2.1-19.6,1.1-25,0.5                                   c-13.3,7.9-27.1,30.5-31.9,40.3 c-4.9,9.8-11.9,26.9-19.3,31.2 c-5.4,3.1-19.4,0.7-21-11.7 c-1.2-9.5,12.7-29.2,23.8-41.6 c11.1-12.4,19-21.5,22.1-25.4 c0.4-1.8,20.3-56.9,33.8-68 c9.6-7.9,21.1-12.2,42.5-13.3 c26.6-1.3,19.6-6.1,41.2-18.8 C191.6,1.4,204.3,14.9,205.2,17.1z
        // M250 150 L150 350 L350 350 Z
        PathInstructions result;

        
        bool parsingPoints = false;
        bool parsingX = false;
        bool parsingY = false;
        std::string x;
        std::string y;
        PathInstruction::Type type;
        PathInstruction::Positioning pos;
        PointFs points;
        for (size_t idx = 0; idx != inValue.size(); ++idx)
        {
            char ch = inValue[idx];
            if (isInstruction(ch))
            { 
                if (parsingPoints)
                {
                    if (parsingY)
                    {
                        addPoint(parsingX, parsingY, x, y, points);
                    }
                    result.push_back(PathInstruction(type, pos, points));
                    points.clear();
                    parsingPoints = false;
                }
                if (getInstructionType(ch, type, pos))
                {
                    parsingPoints = true;
                    parsingX = true;
                }
                else
                {
                    ReportError("Unable to get instruction type!");
                }
            }
            else if (ch == ' ')
            {
                if (parsingX)
                {
                    if (!x.empty())
                    {
                        parsingX = false;
                        parsingY = true;
                    }
                }
                else if (parsingY)
                {
                    if (!y.empty())
                    {
                        addPoint(parsingX, parsingY, x, y, points);
                    }
                }
            }
            else if (isPathSeparator(ch))
            {
                if (parsingX)
                {
                    if (x.empty())
                    {
                        x += ch;
                    }
                    else
                    {
                        parsingX = false;
                        parsingY = true;

                        // minus sign is both separator and first new char of y
                        if (ch == '-')
                        {
                            y += ch;
                        }
                    }
                }
                else if (parsingY)
                {
                    if (y.empty())
                    {
                        y += ch;
                    }
                    else
                    {
                        addPoint(parsingX, parsingY, x, y, points);

                        // minus sign is both separator and first new char of x
                        if (ch == '-')
                        {
                            x += ch;
                        }
                    }
                }
            }
            else if (parsingX)
            {
                x += ch;
            }
            else if (parsingY)
            {
                y += ch;
            }
            else
            {
                assert(false);
            }
        }
        
        if (!y.empty())
        {
            addPoint(parsingX, parsingY, x, y, points);
        }
        if (!points.empty())
        {            
            result.push_back(PathInstruction(type, pos, points));
        }
        return result;
    }


} // namespace XULWin
