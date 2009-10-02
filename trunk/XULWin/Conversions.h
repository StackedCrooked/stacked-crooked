#ifndef CONVERSIONS_H_INCLUDED
#define CONVERSIONS_H_INCLUDED


#include "Enums.h"
#include "Points.h"
#include "RGBColor.h"
#include <string>


namespace XULWin
{

    std::string Int2String(int inValue);
    
    int String2Int(const std::string & inValue, int inDefault);
    
    int String2Int(const std::string & inValue);
    
    bool String2Bool(const std::string & inString, bool inDefault);
    
    std::string Bool2String(bool inBool);
    
    Orient String2Orient(const std::string & inValue, Orient inDefault);
    
    std::string Orient2String(Orient inOrient);
    
    Align String2Align(const std::string & inValue, Align inDefault);
    
    std::string Align2String(Align inAlign);

    int String2CSSSize(const std::string & inString, int inDefault);

    std::string CSSTextAlign2String(CSSTextAlign inTextAlign);

    CSSTextAlign String2CSSTextAlign(const std::string & inTextAlign, CSSTextAlign inDefault);

    std::string CSSOverflow2String(CSSOverflow inTextAlign);

    CSSOverflow String2CSSOverflow(const std::string & inTextAlign, CSSOverflow inDefault);

    std::string RGBColor2String(const RGBColor & inColor);

    // HTML (hex) style formatting, for example: "eb8080"
    bool HTMLColor2RGBColor(const std::string & inValue, RGBColor & outResult);

    // RGB style formatting, for example: "rgb(0,0,255)"
    bool RGBString2RGBColor(const std::string & inValue, RGBColor & outResult);

    bool String2RGBColor(const std::string & inValue, RGBColor & outResult);

    std::string Point2String(const Point & inPoint);

    Point String2Point(const std::string & inValue);

    std::string Points2String(const Points & inPoint);

    Points String2Points(const std::string & inValue);

} // namespace XULWin


#endif // CONVERSIONS_H_INCLUDED
