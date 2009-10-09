#ifndef STYLECONTROLLER_H_INCLUDED
#define STYLECONTROLLER_H_INCLUDED


#include "Enums.h"
#include "RGBColor.h"
#include <string>


namespace XULWin
{

    class StyleController
    {
    public:
        virtual void get(std::string & outValue) = 0;
        virtual void set(const std::string & inValue) = 0;
    };


    class CSSXController : public StyleController
    {
    public:
        static const char * PropertyName() { return "x"; }

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual int getCSSX() const = 0;

        virtual void setCSSX(int inX) = 0;
    };


    class CSSYController : public StyleController
    {
    public:
        static const char * PropertyName() { return "y"; }

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual int getCSSY() const = 0;

        virtual void setCSSY(int inY) = 0;
    };


    class CSSWidthController : public StyleController
    {
    public:
        static const char * PropertyName() { return "width"; }

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual int getCSSWidth() const = 0;

        virtual void setCSSWidth(int inWidth) = 0;
    };


    class CSSHeightController : public StyleController
    {
    public:
        static const char * PropertyName() { return "height"; }

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual int getCSSHeight() const = 0;

        virtual void setCSSHeight(int inHeight) = 0;
    };


    class CSSTextAlignController : public StyleController
    {
    public:
        static const char * PropertyName() { return "text-align"; }

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual CSSTextAlign getCSSTextAlign() const = 0;

        virtual void setCSSTextAlign(CSSTextAlign inValue) = 0;
    };


    class CSSMarginController : public StyleController
    {
    public:
        static const char * PropertyName() { return "margin"; }

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual void getCSSMargin(int & outTop, int & outLeft, int & outRight, int & outBottom) const = 0;

        virtual void setCSSMargin(int inTop, int inLeft, int inRight, int inBottom) = 0;
    };


    class CSSFillController : public StyleController
    {
    public:
        static const char * PropertyName() { return "fill"; }

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual void setCSSFill(const RGBColor & inColor) = 0;

        virtual const RGBColor & getCSSFill() const = 0;
    };


    class CSSStrokeController : public StyleController
    {
    public:
        static const char * PropertyName() { return "stroke"; }

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual void setCSSStroke(const RGBColor & inColor) = 0;

        virtual const RGBColor & getCSSStroke() const = 0;
    };


    class CSSListStyleImageController : public StyleController
    {
    public:
        static const char * PropertyName() { return "list-style-image"; }

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual void setCSSListStyleImage(const std::string & inURL) = 0;

        virtual const std::string & getCSSListStyleImage() const = 0;
    };

    

} // namespace XULWin

#endif // STYLECONTROLLER_H_INCLUDED
