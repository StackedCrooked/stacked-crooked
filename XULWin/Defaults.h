#ifndef DEFAULTS_H_INCLUDED
#define DEFAULTS_H_INCLUDED


namespace XULWin
{

    namespace Defaults
    {
        int windowWidth() { return 320; }

        int windowHeight() { return 240; }

        int controlHeight() { return 20; }

        int textPadding() { return 6; }

        int spacing() { return 2; }

        int dropDownListItemHeight() { return 14; }

        int componentMinimumWidth() { return 80; }

        int componentMinimumHeight() { return 20; }

        int textBoxMinimumWidth() { return textPadding(); }
        
        namespace Attributes
        {
            int flex() { return 0; }            
        } // namespace Attributes

    } // namespace Defaults

} // namespace XULWin


#endif // DEFAULTS_H_INCLUDED
