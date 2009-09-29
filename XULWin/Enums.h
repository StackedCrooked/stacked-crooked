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
        HORIZONTAL,
        VERTICAL
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

    
} // namespace XULWin

#endif // STYLES_H_INCLUDED
