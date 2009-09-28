#ifndef CONVERSIONS_H_INCLUDED
#define CONVERSIONS_H_INCLUDED


#include "Layout.h"
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
    
    int CssString2Size(const std::string & inString, int inDefault);

} // namespace XULWin


#endif // CONVERSIONS_H_INCLUDED
