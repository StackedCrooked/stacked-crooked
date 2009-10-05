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
    
    class SVGPainter
    {
    public:
        virtual void paint(Gdiplus::Graphics & g) = 0;
    };


    class SVGCanvas : public NativeControl,
                      public GdiplusLoader
    {
    public:
        typedef NativeControl Super;

        SVGCanvas(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        void bufferedPaint(HDC inHDC);

        virtual void paint(HDC inHDC);
    };


    class SVGElementImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        SVGElementImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

    private:
    };


    class SVGGroupImpl : public SVGElementImpl,
                         public SVGPainter
    {
    public:
        typedef SVGElementImpl Super;

        SVGGroupImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initStyleControllers();

        virtual void paint(Gdiplus::Graphics & g);
    };


    class SVGPolygonImpl : public SVGElementImpl,
                           public SVGPainter,
                           public virtual PointsController
    {
    public:
        typedef SVGElementImpl Super;

        SVGPolygonImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual const Points & getPoints() const;

        virtual void setPoints(const Points & inPoints);

        virtual void paint(Gdiplus::Graphics & g);

    private:
        Points mPoints;
        std::vector<Gdiplus::PointF> mNativePoints;
    };


    class SVGRectImpl : public SVGElementImpl,
                        public SVGPainter
    {
    public:
        typedef SVGElementImpl Super;

        SVGRectImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initStyleControllers();

        virtual void paint(Gdiplus::Graphics & g);

    private:
        std::vector<Gdiplus::PointF> mNativePoints;
    };


    class SVGPathImpl : public SVGElementImpl,
                        public virtual PathInstructionsController,
                        public SVGPainter
    {
    public:
        typedef SVGElementImpl Super;

        SVGPathImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

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

        //bool getFillColor(Gdiplus::Color & outColor);

        //bool getStrokeColor(Gdiplus::Color & outColor);

        static void GetPreparedInstructions(const PathInstructions & inData, PathInstructions & outPrepData);

        static void GetPointReflection(const PointF & inPoint, const PointF & inOrigin, PointF & outReflection);

        PathInstructions mInstructions;
        PathInstructions mPreparedInstructions;
    };

} // namespace SVG

} // namespace XULWin

#endif // SVGIMPL_H_INCLUDED
