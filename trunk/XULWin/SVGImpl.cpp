#include "SVGImpl.h"
#include "PathInstructions.h"
#include "Utils/ErrorReporter.h"


using namespace Utils;


namespace XULWin
{

namespace SVG
{
    
    
    SVGCanvas::SVGCanvas(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent, inAttributesMapping, TEXT("STATIC"), 0, 0)
    {
    }
        
        
    int SVGCanvas::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return 250;
    }

    
    int SVGCanvas::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return 250;
    }
    
    
    void SVGCanvas::paint(HDC inHDC)
    {
        Gdiplus::Graphics g(inHDC);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            if (SVGPainter * svg = owningElement()->children()[idx]->impl()->downcast<SVGPainter>())
            {
                svg->paint(g);
            }
        }
    }
    
    
    void SVGCanvas::bufferedPaint(HDC inHDC)
    {

        //
        // Get the size of the client rectangle.
        //
        RECT rc;
        GetClientRect(handle(), &rc);
        
        HDC compatibleDC = CreateCompatibleDC(inHDC);
        
        
        //
        // Create a bitmap big enough for our client rectangle.
        //
        HBITMAP backgroundBuffer = CreateCompatibleBitmap(inHDC, rc.right - rc.left, rc.bottom - rc.top);


        //
        // Select the bitmap into the off-screen DC.
        //
        HBITMAP backgroundBitmap = (HBITMAP)SelectObject(compatibleDC, backgroundBuffer);


        //
        // Erase the background.
        //
        HBRUSH backgroundBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        FillRect(compatibleDC, &rc, backgroundBrush);
        DeleteObject(backgroundBrush);

        //
        // Render the image into the offscreen DC.
        //
        SetBkMode(compatibleDC, TRANSPARENT);
        

        paint(compatibleDC);


        //
        // Blt the changes to the screen DC.
        //
        BitBlt
        (
            inHDC,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            compatibleDC, 0, 0, SRCCOPY
        );

        //
        // Done with off-screen bitmap and DC.
        //
        SelectObject(compatibleDC, backgroundBitmap);
        DeleteObject(backgroundBuffer);
        DeleteDC(compatibleDC);
    }

    
    LRESULT SVGCanvas::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        if (inMessage == WM_PAINT)
        {
            HDC hDC = ::GetDC(handle());
            PAINTSTRUCT ps;
            ps.hdc = hDC;
            ::BeginPaint(handle(), &ps);
            bufferedPaint(hDC);
            ::EndPaint(handle(), &ps);
            ::ReleaseDC(handle(), hDC);
            return TRUE;
        }
        return Super::handleMessage(inMessage, wParam, lParam);
    }


    SVGElementImpl::SVGElementImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        PassiveComponent(inParent, inAttributesMapping)
    {
    }

        
    static SVGElementImpl * findSVGParent(ElementImpl * inEl)
    {
        if (!inEl)
        {
            return 0;
        }

        if (SVGElementImpl * g = inEl->downcast<SVGElementImpl>())
        {
            return g;
        }
        else
        {
            return findSVGParent(inEl->parent());
        }
    }


    SVGGroupImpl::SVGGroupImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        SVGElementImpl(inParent, inAttributesMapping)
    {
    }


    bool SVGGroupImpl::initStyleControllers()
    {
        setStyleController(CSSFillController::PropertyName(), static_cast<CSSFillController*>(this));
        return Super::initStyleControllers();
    }
    
    
    void SVGGroupImpl::paint(Gdiplus::Graphics & g)
    {
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            if (SVGPainter * svg = owningElement()->children()[idx]->impl()->downcast<SVGPainter>())
            {
                svg->paint(g);
            }
        }
    }

    
    SVGPolygonImpl::SVGPolygonImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        SVGElementImpl(inParent, inAttributesMapping)
    {
    }
        
        
    bool SVGPolygonImpl::initAttributeControllers()
    {
        setAttributeController("points", static_cast<PointsController*>(this));
        return Super::initAttributeControllers();
    }


    const Points & SVGPolygonImpl::getPoints() const
    {
        return mPoints;
    }


    void SVGPolygonImpl::setPoints(const Points & inPoints)
    {
        mPoints = inPoints;
        mNativePoints.clear();
        for (size_t idx = 0; idx != mPoints.size(); ++idx)
        {
            const Point & point = mPoints[idx];
            mNativePoints.push_back(Gdiplus::PointF((Gdiplus::REAL)point.x(), (Gdiplus::REAL)point.y()));
        }
    }

    
    void SVGPolygonImpl::paint(Gdiplus::Graphics & g)
    {
        if (!mNativePoints.empty())
        {
            Gdiplus::Color color(Gdiplus::Color::Black);
            SVGElementImpl * svg = findSVGParent(this);
            if (svg)
            {
                RGBColor fill = svg->getCSSFill();
                color = Gdiplus::Color(fill.alpha(), fill.red(), fill.green(), fill.blue());
            }
            Gdiplus::SolidBrush solidBrush(color);
            g.FillPolygon(&solidBrush, &mNativePoints[0], mNativePoints.size());
        }
    }


    SVGRectImpl::SVGRectImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        SVGElementImpl(inParent, inAttributesMapping)
    {
    }


    bool SVGRectImpl::initStyleControllers()
    {
        setStyleController("x", static_cast<CSSXController*>(this));
        setStyleController("y", static_cast<CSSYController*>(this));
        setStyleController("width", static_cast<CSSWidthController*>(this));
        setStyleController("height", static_cast<CSSHeightController*>(this));
        setStyleController("fill", static_cast<CSSFillController*>(this));
        return Super::initStyleControllers();
    }


    void SVGRectImpl::paint(Gdiplus::Graphics & g)
    {
        Gdiplus::Color color(Gdiplus::Color::Black);
        RGBColor fill = getFill();
        color = Gdiplus::Color(fill.alpha(), fill.red(), fill.green(), fill.blue());
        Gdiplus::SolidBrush solidBrush(color);
        g.FillRectangle(&solidBrush, Gdiplus::RectF((Gdiplus::REAL)getCSSX(),
                                                    (Gdiplus::REAL)getCSSY(),
                                                    (Gdiplus::REAL)getWidth(),
                                                    (Gdiplus::REAL)getHeight()));
    }

    
    SVGPathImpl::SVGPathImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        SVGElementImpl(inParent, inAttributesMapping)
    {
    }
        
        
    bool SVGPathImpl::initAttributeControllers()
    {
        setAttributeController("d", static_cast<PathInstructionsController*>(this));
        return Super::initAttributeControllers();
    }
        
        
    bool SVGPathImpl::initStyleControllers()
    {
        return Super::initStyleControllers();
    }


    void SVGPathImpl::getFloatPoints(const PathInstruction & instruction,
                                    const Gdiplus::PointF & inPrevPoint,
                                    std::vector<Gdiplus::PointF> & outPoints)
    {
        for (size_t idx = 0; idx != instruction.numPoints(); ++idx)
        {
            const PointF & point = instruction.getPoint(idx);
            Gdiplus::PointF pointF;
            if (instruction.positioning() == PathInstruction::Absolute)
            {
                pointF = Gdiplus::PointF((Gdiplus::REAL)point.x(),
                                         (Gdiplus::REAL)point.y());
            }
            else
            {
                pointF = Gdiplus::PointF(inPrevPoint.X + (Gdiplus::REAL)point.x(),
                                         inPrevPoint.Y + (Gdiplus::REAL)point.y());
            }
            outPoints.push_back(pointF);
        }
    }


    void SVGPathImpl::GetAbsolutePositions(const PointFs & inRelativePoints,
                                          const PointF & inPrevPoint,
                                          PointFs & outPoints)
    {
        for (size_t idx = 0; idx != inRelativePoints.size(); ++idx)
        {
            const PointF & point = inRelativePoints[idx];
            PointF pointF(inPrevPoint.x() + point.x(), inPrevPoint.y() + point.y());
            outPoints.push_back(pointF);
        }
    }


    void SVGPathImpl::GetAbsolutePositions(const PathInstruction & instruction,
                                          const PointF & inPrevPoint,
                                          PointFs & outPoints)
    {
        for (size_t idx = 0; idx != instruction.numPoints(); ++idx)
        {
            const PointF & point = instruction.getPoint(idx);
            PointF pointF;
            if (instruction.positioning() == PathInstruction::Absolute)
            {
                pointF = PointF(point.x(), point.y());
            }
            else
            {
                pointF = PointF(inPrevPoint.x() + point.x(), inPrevPoint.y() + point.y());
            }
            outPoints.push_back(pointF);
        }
    }


    void SVGPathImpl::GetPointReflection(const PointF & inPoint,
                                        const PointF & inOrigin,
                                        PointF & outReflection)
    {
        float x = inOrigin.x() - inPoint.x();
        float y = inOrigin.y() - inPoint.y();
        outReflection = PointF(x, y);
    }

    
    void SVGPathImpl::GetPreparedInstructions(const PathInstructions & inData, PathInstructions & outPrepData)
    {
        PointFs preppedPoints;
        PointF prevPoint;
        PathInstruction prevInstruction(PathInstruction::MoveTo,
                                        PathInstruction::Relative,
                                        PointFs());
        for (size_t idx = 0; idx != inData.size(); ++idx)
        {
            const PathInstruction & instruction = inData[idx];
            switch (instruction.type())
            {
                case PathInstruction::MoveTo: // M
                {
                    if (!preppedPoints.empty())
                    {
                        outPrepData.push_back(PathInstruction(prevInstruction.type(),
                                                              PathInstruction::Absolute,
                                                              preppedPoints));
                        preppedPoints.clear();
                    }
                    if (instruction.numPoints() == 1)
                    {
                        GetAbsolutePositions(instruction, prevPoint, preppedPoints);
                        if (!preppedPoints.empty())
                        {
                            prevPoint = preppedPoints[preppedPoints.size() - 1];
                        }
                    }
                    else
                    {
                        ReportError("SVG Path of type MoveTo has more than one points.");
                    }
                    break;
                }
                case PathInstruction::LineTo: // L
                case PathInstruction::HorizontalLineTo: // H
                case PathInstruction::VerticalLineTo: // V
                {
                    if (!preppedPoints.empty() && prevInstruction.type() != PathInstruction::MoveTo)
                    {
                        outPrepData.push_back(PathInstruction(prevInstruction.type(),
                                                              PathInstruction::Absolute,
                                                              preppedPoints));
                        if (!preppedPoints.empty())
                        {
                            prevPoint = preppedPoints[preppedPoints.size() - 1];
                        }
                        preppedPoints.clear();
                    }
                    if (instruction.numPoints() == 1)
                    {
                        if (preppedPoints.empty())
                        {
                            preppedPoints.push_back(prevPoint);
                        }
                        GetAbsolutePositions(instruction, prevPoint, preppedPoints);
                        outPrepData.push_back(PathInstruction(instruction.type(),
                                                              PathInstruction::Absolute,
                                                              preppedPoints));
                        if (!preppedPoints.empty())
                        {
                            prevPoint = preppedPoints[preppedPoints.size() - 1];
                        }
                        preppedPoints.clear();
                    }
                    else
                    {
                        ReportError("SVG Path of type LineTo, HorizontalLineTo or VerticalLineTo has more than one points.");
                    }
                    break;
                }
                case PathInstruction::CurveTo: // C
                {
                    if (preppedPoints.empty())
                    {
                        // If preppedPoints is empty at this point then we need
                        // to add the begin point ourselves. We do this after
                        // the call to GetAbsolutePositions because prevPoint
                        // is already absolute positioned.
                        GetAbsolutePositions(instruction, prevPoint, preppedPoints);
                        preppedPoints.insert(preppedPoints.begin(), prevPoint);
                    }
                    else
                    {
                        GetAbsolutePositions(instruction, prevPoint, preppedPoints);
                    }
                    if (!preppedPoints.empty())
                    {
                        prevPoint = preppedPoints[preppedPoints.size() - 1];
                    }
                    break;
                }
                case PathInstruction::SmoothCurveTo: // S
                {
                    // We'll convert it to a curve instruction
                    PathInstruction curveInstruction = instruction;
                    if (prevInstruction.type() == PathInstruction::CurveTo)
                    {  
                        PointF c2 = instruction.getPoint(0);
                        PointF endPoint = instruction.getPoint(1);

                        // S produces the same type of curve as C, but if it
                        // follows another S command or a C command, the first 
                        // control point is assumed to be a reflection of the 
                        // one used previously.
                        PointF c1;
                        if (instruction.positioning() == PathInstruction::Relative)
                        {
                            GetPointReflection(prevInstruction.getPoint(1),
                                               prevInstruction.getPoint(2),
                                               c1);
                        }
                        else
                        {
                            GetPointReflection(prevInstruction.getAbsolutePoint(prevPoint, 1),
                                               prevInstruction.getAbsolutePoint(prevPoint, 2),
                                               c1);
                            // now also make it absolute position
                            c1 = PointF(prevPoint.x() + c1.x(), prevPoint.y() + c1.y());
                        }
                        curveInstruction.points().insert(curveInstruction.points().begin(), c1);
                    }
                    else if (prevInstruction.type() == PathInstruction::SmoothCurveTo)
                    {
                        PointF c2 = instruction.getPoint(0);
                        PointF endPoint = instruction.getPoint(1);

                        // S produces the same type of curve as C, but if it
                        // follows another S command or a C command, the first 
                        // control point is assumed to be a reflection of the 
                        // one used previously.
                        PointF c1;
                        if (instruction.positioning() == PathInstruction::Relative)
                        {
                            GetPointReflection(prevInstruction.getPoint(0),
                                               prevInstruction.getPoint(1),
                                               c1);
                        }
                        else
                        {
                            GetPointReflection(prevInstruction.getAbsolutePoint(prevPoint, 0),
                                               prevInstruction.getAbsolutePoint(prevPoint, 1),
                                               c1);
                            // now also make it absolute position
                            c1 = PointF(prevPoint.x() + c1.x(), prevPoint.y() + c1.y());
                        }
                        curveInstruction.points().insert(curveInstruction.points().begin(), c1);
                    }
                    else
                    {
                        //"M186.2,26
                        //s-2.3, 5.9 -5.2, 5.9
                        //S186.2,29.2 186.2,26z"
                        // If the S command doesn't follow another S or C command, then it is
                        // assumed that both control points for the curve are the same.
                        if (instruction.positioning() == PathInstruction::Relative)
                        {
                            curveInstruction.points().insert(curveInstruction.points().begin(),
                                                             instruction.getPoint(0));
                        }
                        else
                        {
                            curveInstruction.points().insert(curveInstruction.points().begin(),
                                                             instruction.getAbsolutePoint(prevPoint, 0));
                        }
                    }
                    if (preppedPoints.empty())
                    {
                        // If preppedPoints is empty at this point then we need
                        // to add the begin point ourselves. We do this after
                        // the call to GetAbsolutePositions because prevPoint
                        // is already absolute positioned.
                        GetAbsolutePositions(curveInstruction, prevPoint, preppedPoints);
                        preppedPoints.insert(preppedPoints.begin(), prevPoint);
                    }
                    else
                    {
                        GetAbsolutePositions(curveInstruction, prevPoint, preppedPoints);
                    }
                    if (!preppedPoints.empty())
                    {
                        prevPoint = preppedPoints[preppedPoints.size() - 1];
                    }
                    break;
                }
                case PathInstruction::QuadraticBelzierCurve: // Q
                case PathInstruction::SmoothQuadraticBelzierCurveTo: // T
                case PathInstruction::EllipticalArc: // A
                {
                    assert(false); // not yet implemented
                    break;
                }
                case PathInstruction::ClosePath: // Z
                {
                    if (!preppedPoints.empty())
                    {
                        outPrepData.push_back(PathInstruction(prevInstruction.type(),
                                                              PathInstruction::Absolute,
                                                              preppedPoints));
                        prevPoint = preppedPoints[preppedPoints.size() - 1];
                        preppedPoints.clear();
                    }
                    // This command draws a straight line from your current
                    // position back to the first point that started the path.
                    if (!inData.empty())
                    {
                        if (!inData[0].points().empty())
                        {
                            preppedPoints.push_back(inData[0].points()[0]);
                            preppedPoints.push_back(prevPoint);
                            if (preppedPoints[0] != preppedPoints[1])
                            {
                                outPrepData.push_back(PathInstruction(PathInstruction::LineTo,
                                                                      PathInstruction::Absolute,
                                                                      preppedPoints));
                            }
                            preppedPoints.clear();
                        }
                    }
                    else
                    {
                        // How could inData be empty if we are inside the for loop??
                        assert(false); 
                    }
                    break;
                }
            }

            prevInstruction = instruction;
        }
        if (!preppedPoints.empty())
        {
            outPrepData.push_back(PathInstruction(prevInstruction.type(),
                                                  PathInstruction::Absolute,
                                                  preppedPoints));
            preppedPoints.clear();
        }
    }

    
    void SVGPathImpl::paint(Gdiplus::Graphics & g)
    {
        RGBColor fillColorRGB(getFill());
        Gdiplus::Color fillColor(fillColorRGB.alpha(),
                                 fillColorRGB.red(),
                                 fillColorRGB.green(),
                                 fillColorRGB.blue());
        Gdiplus::SolidBrush brush(fillColor);


        RGBColor strokeColorRGB(getStroke());
        Gdiplus::Color strokeColor(strokeColorRGB.alpha(),
                                   strokeColorRGB.red(),
                                   strokeColorRGB.green(),
                                   strokeColorRGB.blue());
        Gdiplus::Pen pen(strokeColor, 3.0f);

        Gdiplus::GraphicsPath path;
        path.SetFillMode(Gdiplus::FillModeWinding);
        for (size_t idx = 0; idx != mPreparedInstructions.size(); ++idx)
        {
            const PathInstruction & instruction = mPreparedInstructions[idx];
            const PointFs & points = instruction.points();
            switch (instruction.type())
            {
                case PathInstruction::MoveTo: // should not be found in mPreparedInstructions
                {
                    assert(false);
                    break;
                }
                case PathInstruction::LineTo:
                {
                    if (instruction.numPoints() == 2)
                    {
                        path.AddLine(Gdiplus::PointF(instruction.getPoint(0).x(), instruction.getPoint(0).y()),
                                     Gdiplus::PointF(instruction.getPoint(1).x(), instruction.getPoint(1).y()));
                    }
                    else
                    {
                        ReportError("LineTo: paint failed because number of prepared points is not equal to 2.");
                    }
                    break;
                }
                case PathInstruction::HorizontalLineTo:
                case PathInstruction::VerticalLineTo:
                {
                    assert(false); // not yet supported
                    break;
                }
                case PathInstruction::CurveTo:
                case PathInstruction::SmoothCurveTo:
                {
                    std::vector<Gdiplus::PointF> gdiplusPoints;
                    for (size_t idx = 0; idx != points.size(); ++idx)
                    {
                        const PointF & point = points[idx];
                        gdiplusPoints.push_back(Gdiplus::PointF(point.x(), point.y()));
                    }
                    path.AddBeziers(&gdiplusPoints[0], gdiplusPoints.size());
                    break;
                }
                case PathInstruction::QuadraticBelzierCurve:
                case PathInstruction::SmoothQuadraticBelzierCurveTo:
                case PathInstruction::EllipticalArc:
                {
                    assert(false); // not yet implemented
                    break;
                }
                case PathInstruction::ClosePath: // should not be found in mPreparedInstructions
                {
                    assert(false); 
                    break;
                }
                default:
                {
                    assert(false);
                    ReportError("Unsupported type");
                }
            }
        }
        g.FillPath(&brush, &path);
        g.DrawPath(&pen, &path);
    }
    
    
    const PathInstructions & SVGPathImpl::getPathInstructions() const
    {
        return mInstructions;
    }


    void SVGPathImpl::setPathInstructions(const PathInstructions & inPathInstructions)
    {
        mInstructions = inPathInstructions;
        mPreparedInstructions.clear();
        GetPreparedInstructions(mInstructions, mPreparedInstructions);
    }


} // namespace SVG

} // namespace XULWin
