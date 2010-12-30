#include "HSVColor.h"
#include "RGBColor.h"
#include <cassert>
#include <cmath>


namespace XULWin {


HSVColor::HSVColor() :
    mHue(0),
    mSaturation(0),
    mValue(0)
{
}


HSVColor::HSVColor(int inHue, int inSaturation, int inValue) :
    mHue(inHue),
    mSaturation(inSaturation),
    mValue(inValue)

{
}


int HSVColor::hue() const
{
    return mHue;
}


int HSVColor::saturation() const
{
    return mSaturation;
}


int HSVColor::value() const
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
	double hue = 0;
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

	double saturation = 0;
    if (maxColor != 0)
    {
        saturation = chroma / maxColor;
    }
	assert(saturation >= 0 && saturation <= 1);

	double value = maxColor;
	assert(value >= 0 && value <= 1);

	return HSVColor(static_cast<int>(0.5 + hue),
				    static_cast<int>(0.5 + 100.0 * saturation),
					static_cast<int>(0.5 + 100.0 * value));
}


RGBColor HSV2RGB(const HSVColor & hsv)
{
    // Convert from percentage to decimal in [0..1] range.
    double s = static_cast<double>(hsv.saturation()) / 100.0;
    double v = static_cast<double>(hsv.value()) / 100.0;

	if (hsv.saturation() == 0)
	{
		int color = static_cast<int>(0.5 + 255.0 * v);
		return RGBColor(color, color, color);
	}


	double h = hsv.hue() / 60.0;
	double i = std::floor(h);
	double f = h - i;
	double p = v * (1.0 - s);
	double q = v * (1.0 - f * s);
    double t = v * (1.0 - (1.0 - f) * s);

	double r, g, b;
	assert(static_cast<int>(i) >= 0 && static_cast<int>(i) <= 5);
	switch (static_cast<int>(i))
	{
		case 0: // Vtp
		{
			r = v;
			g = t;
			b = p;
			break;
		}
		case 1: // qVp
		{
			r = q;
			g = v;
			b = p;
			break;
		}
		case 2: // pVt
		{
			r = p;
			g = v;
			b = t;
			break;
		}
		case 3: // pqV
		{
			r = p;
			g = q;
			b = v;
			break;
		}
		case 4: // tpV
		{
			r = t;
			g = p;
			b = v;
			break;
		}
		default: // case 5, Vpq
		{
			r = v;
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

