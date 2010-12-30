#ifndef HSV_COLOR_H_INCLUDED
#define HSV_COLOR_H_INCLUDED


#include "RGBColor.h"
#include <algorithm>


namespace XULWin {


class HSVColor
{
public:
    HSVColor();

	HSVColor(int inHue, int inSaturation, int inValue);

	bool operator==(const HSVColor & rhs) const
	{ return mHue == rhs.mHue && mSaturation == rhs.mSaturation && mValue == rhs.mValue; }

	bool operator!=(const HSVColor & rhs) const
	{ return !(*this == rhs); }

    int hue() const;

    int saturation() const;

    int value() const;

private:
    int mHue;
    int mSaturation;
    int mValue;
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

