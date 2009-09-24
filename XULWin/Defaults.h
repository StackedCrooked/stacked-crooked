#ifndef DEFAULTS_H_INCLUDED
#define DEFAULTS_H_INCLUDED


namespace XULWin
{

    namespace Defaults
    {
        static std::string locale() { return "en"; }

        static int windowWidth() { return 320; }

        static int windowHeight() { return 240; }

        static int controlHeight() { return 21; }

        static int buttonHeight() { return 23; }

        static int textPadding() { return 12; }

        static int dropDownListMinimumWidth() { return 24; }

        static int dropDownListItemHeight() { return 16; }

        static int componentMinimumWidth() { return 80; }

        static int componentMinimumHeight() { return 20; }

        static int radioButtonMinimumWidth() { return 20; }

        static int textBoxMinimumWidth() { return textPadding(); }

        static int checkBoxMinimumWidth() { return 20; }


        namespace Attributes
        {

            static int flex() { return 0; }

        } // namespace Attributes

    } // namespace Defaults

} // namespace XULWin


#endif // DEFAULTS_H_INCLUDED
