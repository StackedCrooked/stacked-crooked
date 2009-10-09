#ifndef DEFAULTS_H_INCLUDED
#define DEFAULTS_H_INCLUDED


#include "Enums.h"


namespace XULWin
{

    namespace Defaults
    {
        static std::string locale() { return "en-US"; }

        static int windowWidth() { return 320; }

        static int windowHeight() { return 240; }

        static int controlHeight() { return 21; }

        // default control width is rarely used since
        // we usually calculate the minimum width
        static int controlWidth() { return 40; }        

        static int buttonHeight() { return 23; }      

        static int buttonWidth() { return 75; }       

        static int tabHeight() { return 23; } 

        static int textPadding() { return 12; }

        static int dropDownListMinimumWidth() { return 24; }

        static int dropDownListItemHeight() { return 16; }

        static int componentMinimumWidth() { return 80; }

        static int componentMinimumHeight() { return 20; }

        static int radioButtonMinimumWidth() { return 20; }

        static int textBoxMinimumWidth() { return textPadding(); }

        static int checkBoxMinimumWidth() { return 20; }

        static int scrollbarWidth() { return 17; }

        static int progressMeterWidth() { return 125; }

        static int progressMeterHeight() { return 17; }

        static int treeIndent() { return 18; }

        static int treeItemHeight() { return 16; }

        static int statusBarHeight() { return 14; }

        static int toolbarHeight() { return 50; }

        namespace Attributes
        {
            static bool disabled() { return false; }

            static bool readonly() { return false; }

            static bool checked() { return false; } // checkbox

            static Orient orient() { return Vertical; }

            static Align align() { return Stretch; }

            static int flex() { return 0; }

            static int pageincrement() { return 10; }

            static int curpos() { return 0; }

            static int maxpos() { return 100; }

        } // namespace Attributes

    } // namespace Defaults

} // namespace XULWin


#endif // DEFAULTS_H_INCLUDED
