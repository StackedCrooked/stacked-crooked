#ifndef RGB_COLOR_H_INCLUDED
#define RGB_COLOR_H_INCLUDED


namespace XULWin
{

    class RGBColor
    {
    public:
        RGBColor();

        RGBColor(int inRed, int inGreen, int inBlue);

        RGBColor(int inAlpha, int inRed, int inGreen, int inBlue);

        int red() const;

        int green() const;

        int blue() const;

        int alpha() const;

    private:
        int mAlpha;
        int mRed;
        int mGreen;
        int mBlue;
    };

} // namespace XULWin

#endif //RGB_COLOR_H_INCLUDED
