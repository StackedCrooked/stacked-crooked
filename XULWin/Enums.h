#ifndef STYLES_H_INCLUDED
#define STYLES_H_INCLUDED


#include <string>


namespace XULWin
{

    enum SizeConstraint
    {
        Minimum,
        Optimal,
        Maximum
    };


    enum Orient
    {
        Horizontal,
        Vertical
    };


    enum Align
    {
        Start,
        Center,
        End,
        Stretch
    };

    enum CSSTextAlign
    {
        CSSTextAlign_Left,
        CSSTextAlign_Center,
        CSSTextAlign_Right,
        //CSSTextAlign_Justify // not supported atm
    };

    enum CSSOverflow
    {
        CSSOverflow_Visible,
        CSSOverflow_Hidden,
        CSSOverflow_Scroll,
        CSSOverflow_Auto
    };

    
} // namespace XULWin

#endif // STYLES_H_INCLUDED
