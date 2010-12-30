#include "RGBColor.h"
#include "HSVColor.h"
#include <iomanip>
#include <iostream>
#include <utility>


using namespace XULWin;


inline std::ostream & operator<<(std::ostream & os, const RGBColor & inRGBColor)
{
	return (os << "RGB(" << std::setw(3) << std::setw(3) << inRGBColor.red() << ", " << std::setw(3) << inRGBColor.green() << ", " << std::setw(3) << inRGBColor.blue() << ")");
}


inline std::ostream & operator<<(std::ostream & os, const HSVColor & inHSVColor)
{
	return (os << "HSV(" << std::setw(3) << inHSVColor.hue() << ", " << std::setw(3) << inHSVColor.saturation() << ", " << std::setw(3) << inHSVColor.value() << ")");
}


void Test(const RGBColor & inRGBColor, const HSVColor & inCheck)
{
	HSVColor hsv(RGB2HSV(inRGBColor));
	std::cout << inRGBColor << " => " << hsv;
	if (hsv != inCheck)
	{
		std::cout << "\tExpected: " << inCheck;
	}
    else
    {
        std::cout << "\tOK";
    }
	std::cout << std::endl;
}


void Test(const HSVColor & inHSVClor, const RGBColor & inCheck)
{
	RGBColor rgb(HSV2RGB(inHSVClor));
	std::cout << inHSVClor << " => " << rgb;
	if (rgb != inCheck)
	{
		std::cout << "\tExpected: " << inCheck;
	}
    else
    {
        std::cout << "\tOK";
    }
	std::cout << std::endl;
}



typedef std::pair<RGBColor, HSVColor> Entry;

const Entry cEntries[] =
{
	Entry(RGBColor(  0,   0,   0), HSVColor(  0,   0,   0)),
	Entry(RGBColor(255,   0,   0), HSVColor(  0, 100, 100)),
	Entry(RGBColor(  0, 255,   0), HSVColor(120, 100, 100)),
	Entry(RGBColor(  0,   0, 255), HSVColor(240, 100, 100)),
	Entry(RGBColor(255, 255,   0), HSVColor( 60, 100, 100)),
	Entry(RGBColor(  0, 255, 255), HSVColor(180, 100, 100)),
	Entry(RGBColor(255,   0, 255), HSVColor(300, 100, 100)),
	Entry(RGBColor(255, 255, 255), HSVColor(  0,   0, 100)),
	Entry(RGBColor(  1,   2,   3), HSVColor(210,  67,   1)),
	Entry(RGBColor( 10,  20,  30), HSVColor(210,  67,  12))
};

const unsigned int cEntryCount = sizeof(cEntries)/sizeof(cEntries[0]);


int main()
{
    // Test RGB => HSV
	for (unsigned int i = 0; i < cEntryCount; ++i)
	{
		const Entry & entry = cEntries[i];
        Test(entry.first, entry.second);
	}

    // Test HSV => RGB
	for (unsigned int i = 0; i < cEntryCount; ++i)
	{
		const Entry & entry = cEntries[i];
		Test(entry.second, entry.first);
    }
	return 0;
}

