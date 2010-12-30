#include "XULWin/HSVColor.h"
#include "XULWin/RGBColor.h"
#include <cassert>
#include <cmath>


namespace XULWin {


HSVColor::HSVColor() :
    mHue(0),
    mSaturation(0),
    mValue(0)
{
}


HSVColor::HSVColor(float inHue, float inSaturation, float inValue) :
    mHue(inHue),
    mSaturation(inSaturation),
    mValue(inValue)

{
}


float HSVColor::hue() const
{
    return mHue;
}


float HSVColor::saturation() const
{
    return mSaturation;
}


float HSVColor::value() const
{
    return mValue;
}


HSVColor RGB2HSV(const RGBColor & rgb)
{
	double r = rgb.red() / 255.0;
	double g = rgb.green() / 255.0;
	double b = rgb.blue() / 255.0;

	// Calculate chroma
	double maxColor = max(r, g, b);
	double minColor = min(r, g, b);
	double chroma = maxColor - minColor;
	
	// Calculate hue
	double hue = -1; // mark as undefined
	if (chroma != 0)
	{
		if (maxColor == r)
		{
			hue = (g - b / chroma) * 60.0;
		}
		else if (maxColor == g)
		{
			hue = (2 + (b - r) / chroma) * 60.0;
		}
		else // maxColor == b
		{
			assert(maxColor == b);
			hue = (4 + (r - g) / chroma) * 60.0;
		}
		if (hue < 0)
		{
			hue += 360.0;
		}
		assert(hue >= 0 && hue < 360);
	}

	double saturation = chroma / maxColor;
	assert(saturation >= 0 && saturation <= 1);

	double value = maxColor;
	assert(value >= 0 && value <= 1);

	return HSVColor(static_cast<float>(hue),
				    static_cast<float>(saturation),
					static_cast<float>(value));
}


RGBColor HSV2RGB(const HSVColor & hsv)
{
	if (hsv.saturation() == 0)
	{
		int color = static_cast<int>(0.5 + hsv.value());
		return RGBColor(color, color, color);
	}

	double h = hsv.hue() / 60.0;
	double i = std::floor(h);
	double f = h - i;
	double p = hsv.value() * (1.0 - hsv.saturation());
	double q = hsv.value() * (1.0 - hsv.saturation() * f);
	double t = hsv.value() * (1.0 - hsv.saturation() * (1.0 - f));

	double r, g, b;
	assert(static_cast<int>(i) >= 0 && static_cast<int>(i) <= 5);
	switch (static_cast<int>(i))
	{
		case 0: // Vtp
		{
			r = hsv.value();
			g = t;
			b = p;
			break;
		}
		case 1: // qVp
		{
			r = q;
			g = hsv.value();
			b = p;
			break;
		}
		case 2: // pVt
		{
			r = p;
			g = hsv.value();
			b = t;
			break;
		}
		case 3: // pqV
		{
			r = p;
			g = q;
			b = hsv.value();
			break;
		}
		case 4: // tpV
		{
			r = t;
			g = p;
			b = hsv.value();
			break;
		}
		default: // case 5, Vpq
		{
			r = hsv.value();
			g = p;
			b = q;
			break;
		}
	}

	// Convert color values back from hex.
	return RGBColor(static_cast<int>(0.5 + (r * 255.0)),
					static_cast<int>(0.5 + (g * 255.0)),
					static_cast<int>(0.5 + (b * 255.0)));
}


} // namespace XULWin
