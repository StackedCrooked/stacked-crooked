#ifndef SVGIMPL_H_INCLUDED
#define SVGIMPL_H_INCLUDED


#include "ElementImpl.h"
#include "Decorator.h"
#include "Graphics.h"
#include <gdiplus.h>


namespace XULWin
{

namespace SVG
{
    
    class Painter
    {
    public:
        virtual void paint(Gdiplus::Graphics & g) = 0;
    };


    class NativeSVG : public NativeControl,
                      public GdiplusLoader
    {
    public:
        typedef NativeControl Super;

        NativeSVG(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        void bufferedPaint(HDC inHDC);

        virtual void paint(HDC inHDC);
    };


    class NativeG : public PassiveComponent,
                    public Painter,
                    public CSSFillController,
                    public CSSStrokeController
    {
    public:
        typedef PassiveComponent Super;

        NativeG(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initStyleControllers();

        virtual void setCSSFill(const RGBColor & inColor);

        virtual const RGBColor & getCSSFill() const;

        virtual void setCSSStroke(const RGBColor & inColor);

        virtual const RGBColor & getCSSStroke() const;

        virtual void paint(Gdiplus::Graphics & g);

    private:
        Utils::Fallible<RGBColor> mCSSFill;
        Utils::Fallible<RGBColor> mCSSStroke;
    };


    class NativePolygon : public VirtualBox,
                          public Painter,
                          public virtual PointsController
    {
    public:
        typedef VirtualBox Super;

        NativePolygon(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual const Points & getPoints() const;

        virtual void setPoints(const Points & inPoints);

        virtual void paint(Gdiplus::Graphics & g);

    private:
        Points mPoints;
        std::vector<Gdiplus::PointF> mNativePoints;
    };


    class RectImpl : public VirtualBox,
                     public Painter,
                     public virtual WidthController,
                     public virtual HeightController,
                     public virtual CSSXController,
                     public virtual CSSYController,
                     public virtual CSSWidthController,
                     public virtual CSSHeightController,
                     public virtual CSSFillController
    {
    public:
        typedef VirtualBox Super;

        RectImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initStyleControllers();

        virtual void paint(Gdiplus::Graphics & g);

        virtual int getCSSX() const;

        virtual void setCSSX(int inX);

        virtual int getCSSY() const;

        virtual void setCSSY(int inY);

        virtual int getCSSWidth() const;

        virtual void setCSSWidth(int inWidth);

        virtual int getCSSHeight() const;

        virtual void setCSSHeight(int inHeight);

        virtual void setCSSFill(const RGBColor & inColor);

        virtual const RGBColor & getCSSFill() const;

        virtual int getWidth() const;

        virtual void setWidth(int inWidth);

        virtual int getHeight() const;

        virtual void setHeight(int inHeight);

    private:
        int mX;
        int mY;
        int mWidth;
        int mHeight;
        Utils::Fallible<RGBColor> mFill;
        std::vector<Gdiplus::PointF> mNativePoints;
    };


    class NativePath : public VirtualBox,
                       public virtual PathInstructionsController,
                       public virtual FillController,
                       public virtual CSSFillController,
                       public virtual StrokeController,
                       public virtual CSSStrokeController,
                       public Painter
    {
    public:
        typedef VirtualBox Super;

        NativePath(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual void setFill(const RGBColor & inColor);

        virtual const RGBColor & getFill() const;

        virtual void setCSSFill(const RGBColor & inColor);

        virtual const RGBColor & getCSSFill() const;

        virtual void setStroke(const RGBColor & inColor);

        virtual const RGBColor & getStroke() const;

        virtual void setCSSStroke(const RGBColor & inColor);

        virtual const RGBColor & getCSSStroke() const;

        virtual void paint(Gdiplus::Graphics & g);

        virtual const PathInstructions & getPathInstructions() const;

        virtual void setPathInstructions(const PathInstructions & inPathInstructions);

    private:        
        void getFloatPoints(const PathInstruction & instruction,
                            const Gdiplus::PointF & inPrevPoint,
                            std::vector<Gdiplus::PointF> & outPoints);

        static void GetAbsolutePositions(const PathInstruction & instruction,
                                         const PointF & inPrevPoint,
                                         PointFs & outPoints);

        static void GetAbsolutePositions(const PointFs & inRelativePoints,
                                         const PointF & inPrevPoint,
                                         PointFs & outPoints);

        bool getFillColor(Gdiplus::Color & outColor);

        bool getStrokeColor(Gdiplus::Color & outColor);

        static void GetPreparedInstructions(const PathInstructions & inData, PathInstructions & outPrepData);

        static void GetPointReflection(const PointF & inPoint, const PointF & inOrigin, PointF & outReflection);

        PathInstructions mInstructions;
        PathInstructions mPreparedInstructions;
        Utils::Fallible<RGBColor> mFill;
        Utils::Fallible<RGBColor> mCSSFill;
        Utils::Fallible<RGBColor> mStroke;
        Utils::Fallible<RGBColor> mCSSStroke;
    };

} // namespace SVG

} // namespace XULWin

#endif // SVGIMPL_H_INCLUDED
