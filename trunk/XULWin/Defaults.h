#ifndef DEFAULTS_H_INCLUDED
#define DEFAULTS_H_INCLUDED


namespace XULWin
{

    namespace Defaults
    {
        int windowWidth() { return 320; }

        int windowHeight() { return 240; }

        int controlHeight() { return 20; }

        int dropDownListItemHeight() { return 20; }

        int componentMinimumWidth() { return 80; }

        int componentMinimumHeight() { return 20; }
        
        namespace Attributes
        {
            int flex() { return 0; }            
        } // namespace Attributes

    } // namespace Defaults

} // namespace XULWin


#endif // DEFAULTS_H_INCLUDED
