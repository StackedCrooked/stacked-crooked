#ifndef HSV_COLOR_H_INCLUDED
#define HSV_COLOR_H_INCLUDED


#include "XULWin/RGBColor.h"
#include <algorithm>


namespace XULWin {


class HSVColor
{
public:
    HSVColor();

	// Use hue value -1 if undefined.
    HSVColor(float inHue, float inSaturation, float inValue);

	// Returns value -1 if undefined
    float hue() const;

    float saturation() const;

    float value() const;

private:
    float mHue;
    float mSaturation;
    float mValue;
};


//
// Convert RGB to HSV
//
HSVColor RGB2HSV(const RGBColor & inRGBColor);


//
// Convert HSV to RGB
//
RGBColor HSV2RGB(const HSVColor & inHSVColor);


//
// Helper function: max(..) with 3 arguments
//
template <typename T>
const T & max(const T& a, const T& b, const T& c)
{
	return std::max<T>(a, std::max<T>(b, c));
}


//
// Helper function: min(..) with 3 arguments
//
template <typename T>
const T & min(const T& a, const T& b, const T& c)
{
	return std::min<T>(a, std::min<T>(b, c));
}


} // namespace XULWin


#endif // HSV_COLOR_H_INCLUDED
