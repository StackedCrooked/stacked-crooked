#include "RGBColor.h"
#include "HSVColor.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>


using namespace XULWin;


inline std::ostream & operator<<(std::ostream & os, const RGBColor & inRGBColor)
{
	return (os << "RGB(" << std::setw(3) << inRGBColor.red()
               << ", "   << std::setw(3) << inRGBColor.green()
               << ", "   << std::setw(3) << inRGBColor.blue() << ")");
}


inline std::ostream & operator<<(std::ostream & os, const HSVColor & inHSVColor)
{
	return (os << "HSV(" << std::setw(3) << inHSVColor.hue()
               << ", " << std::setw(3) << inHSVColor.saturation()
               << ", " << std::setw(3) << inHSVColor.value() << ")");
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
	Entry(RGBColor( 10,  20,  30), HSVColor(210,  67,  12)),
	Entry(RGBColor(255, 165,   0), HSVColor(210,  67,  12)),
	
		
};

const unsigned int cEntryCount = sizeof(cEntries)/sizeof(cEntries[0]);


typedef char BlockType;
enum
{
    BlockType_Nil,
    BlockType_I,
    BlockType_Begin = BlockType_I,
    BlockType_J,
    BlockType_L,
    BlockType_O,
    BlockType_S,
    BlockType_T,
    BlockType_Z,
    BlockType_End
};

std::string ToString(const BlockType & inBlockType)
{
    switch (inBlockType)
    {
        case BlockType_Nil:
        {
            return "Nil";
        }
        case BlockType_I:
        {
            return "I";
        }
        case BlockType_J:
        {
            return "J";
        }
        case BlockType_L:
        {
            return "L";
        }
        case BlockType_O:
        {
            return "O";
        }
        case BlockType_S:
        {
            return "S";
        }
        case BlockType_T:
        {
            return "T";
        }
        case BlockType_Z:
        {
            return "Z";
        }
        default:
        {
            throw 1;
        }
    }
}


const RGBColor & GetColor(BlockType inBlockType)
{
    static const RGBColor fColors[] =
    {
        RGBColor(255, 255, 255),      // Background
        RGBColor(  0, 255, 255),      // I-Shape
        RGBColor(  0, 0,   255),      // J-Shape
        RGBColor(255, 165,   0),      // L-Shape
        RGBColor(255, 255,   0),      // O-Shape
        RGBColor(  0, 255,   0),      // S-Shape
        RGBColor(160,  32, 240),      // T-Shape
        RGBColor(255,   0,   0)       // Z-Shape
    };
    return fColors[static_cast<int>(inBlockType)];
}


RGBColor GetLightColor(const RGBColor & rgb)
{
	HSVColor hsv(XULWin::RGB2HSV(rgb));	
	double newValue = std::min<double>(2.0 * static_cast<double>(hsv.value()), 100.0);
	HSVColor lightHSV(hsv.hue(), hsv.saturation(), static_cast<int>(0.5 + newValue));
	RGBColor lightRGB(HSV2RGB(lightHSV));
	return RGBColor(rgb.alpha(), lightRGB.red(), lightRGB.green(), lightRGB.blue());
}


RGBColor GetDarkColor(const RGBColor & rgb)
{
	HSVColor hsv(XULWin::RGB2HSV(rgb));
	double newValue = 0.5 * static_cast<double>(hsv.value());
	HSVColor darkHSV(hsv.hue(), hsv.saturation(), static_cast<int>(0.5 + newValue));
	RGBColor darkRGB(HSV2RGB(darkHSV));
	return RGBColor(rgb.alpha(), darkRGB.red(), darkRGB.green(), darkRGB.blue());
}


int main()
{
	std::ofstream out("output.txt");
	for (BlockType i = BlockType_Begin; i < BlockType_End; ++i)
    {
		RGBColor rgb(GetColor(i));
		RGBColor lightRGB(GetLightColor(rgb));
		RGBColor darkRGB(GetDarkColor(rgb));
		HSVColor hsv(RGB2HSV(GetColor(i)));
		out << ToString(i) << " Normal:  HSV(" << hsv.hue() << " degrees, " << hsv.saturation() << "%, " << hsv.value() << "%)" << std::endl;
        out << ToString(i) << " Normal:  RGB(" << rgb.red() << ", " << rgb.green() << ", " << rgb.blue() << ")" << std::endl;
		out << ToString(i) << " light: RGB(" << lightRGB.red() << ", " << lightRGB.green() << ", " << lightRGB.blue() << ")" << std::endl;
		out << ToString(i) << " dark: RGB(" << darkRGB.red() << ", " << darkRGB.green() << ", " << darkRGB.blue() << ")" << std::endl;
        out << std::endl;
    }


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
	std::cin.get();
	return 0;
}

