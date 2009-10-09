#include "StyleController.h"
#include "Conversions.h"
#include "Poco/StringTokenizer.h"
#include <sstream>


namespace XULWin
{

    void CSSTextAlignController::get(std::string & outValue)
    {
        outValue = CSSTextAlign2String(getCSSTextAlign());
    }
    
    
    void CSSTextAlignController::set(const std::string & inValue)
    {
        setCSSTextAlign(String2CSSTextAlign(inValue, CSSTextAlign_Left));
    }


    void CSSXController::get(std::string & outValue)
    {
        outValue = Int2String(getCSSX());
    }


    void CSSXController::set(const std::string & inValue)
    {
        setCSSX(String2CSSSize(inValue, 0));
    }


    void CSSYController::get(std::string & outValue)
    {
        outValue = Int2String(getCSSY());
    }


    void CSSYController::set(const std::string & inValue)
    {
        setCSSY(String2CSSSize(inValue, 0));
    }


    void CSSWidthController::get(std::string & outValue)
    {
        outValue = Int2String(getCSSWidth());
    }


    void CSSWidthController::set(const std::string & inValue)
    {
        setCSSWidth(String2CSSSize(inValue, 0));
    }


    void CSSHeightController::get(std::string & outValue)
    {
        outValue = Int2String(getCSSHeight());
    }


    void CSSHeightController::set(const std::string & inValue)
    {
        setCSSHeight(String2CSSSize(inValue, 0));
    }


    void CSSMarginController::get(std::string & outValue)
    {
        int top, left, bottom, right;
        getCSSMargin(top, left, bottom, right);

        // Four values apply to top, right, bottom and left in that order.
        std::stringstream ss;        
        ss << Int2String(top) << "px ";
        ss << Int2String(right) << "px ";
        ss << Int2String(bottom) << "px ";
        ss << Int2String(left) << "px ";
        outValue = ss.str();
    }
    
    
    void CSSMarginController::set(const std::string & inValue)
    {
        int top = 0;
        int left = 0;
        int bottom = 0;
        int right = 0;
        
        Poco::StringTokenizer tokenizer(inValue,
                                        " ",
                                        Poco::StringTokenizer::TOK_IGNORE_EMPTY
                                        | Poco::StringTokenizer::TOK_TRIM);
        Poco::StringTokenizer::Iterator it = tokenizer.begin(), end = tokenizer.end();
        std::vector<int> values;
        for (; it != end; ++it)
        {
            if (!it->empty())
            {
                values.push_back(String2CSSSize(*it, 0));
            }
        }
        if (values.size() == 1)
        {
            // One single value applies to all four sides.
            top = values[0];
            left = values[0];
            bottom = values[0];
            right = values[0];
        }
        else if (values.size() == 2)
        {
            //Two values apply first to top and bottom, the second one to left and right.
            top = values[0];
            bottom = values[0];
            left = values[1];
            right = values[1];
        }
        else if (values.size() == 3)
        {
            //Three values apply first to top, second to left and right and third to bottom.
            top = values[0];
            left = values[1];
            right = values[1];
            bottom = values[2];

        }
        else if (values.size() == 4)
        {
            // Four values apply to top, right, bottom and left in that order.
            top = values[0];
            right = values[1];
            bottom = values[2];
            left = values[3];

        }
        setCSSMargin(top, left, right, bottom);
    }


    void CSSFillController::get(std::string & outValue)
    {
        outValue = RGBColor2String(getCSSFill());
    }


    void CSSFillController::set(const std::string & inValue)
    {
        RGBColor result;
        if (String2RGBColor(inValue, result))
        {
            setCSSFill(result);
        }
    }


    void CSSStrokeController::get(std::string & outValue)
    {
        outValue = RGBColor2String(getCSSStroke());
    }


    void CSSStrokeController::set(const std::string & inValue)
    {
        RGBColor result;
        if (String2RGBColor(inValue, result))
        {
            setCSSStroke(result);
        }
    }


    void CSSListStyleImageController::get(std::string & outValue)
    {
        outValue = String2URL(getCSSListStyleImage());
    }


    void CSSListStyleImageController::set(const std::string & inValue)
    {
        setCSSListStyleImage(URL2String(inValue));
    }


} // namespace XULWin
