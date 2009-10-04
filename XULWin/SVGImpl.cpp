#include "SVGImpl.h"
#include "PathInstructions.h"
#include "Utils/ErrorReporter.h"


using namespace Utils;


namespace XULWin
{

namespace SVG
{
    
    
    NativeSVG::NativeSVG(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent, inAttributesMapping, TEXT("STATIC"), 0, 0)
    {
    }
        
        
    int NativeSVG::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return 250;
    }

    
    int NativeSVG::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return 250;
    }
    
    
    void NativeSVG::paint(HDC inHDC)
    {
        Gdiplus::Graphics g(inHDC);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            if (Painter * svg = owningElement()->children()[idx]->impl()->downcast<Painter>())
            {
                svg->paint(g);
            }
        }
    }
    
    
    void NativeSVG::bufferedPaint(HDC inHDC)
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

    
    LRESULT NativeSVG::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
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


    NativeG::NativeG(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        PassiveComponent(inParent, inAttributesMapping)
    {
    }
    
        
    static NativeG * findNativeGParent(ElementImpl * inEl)
    {
        if (!inEl)
        {
            return 0;
        }

        if (NativeG * g = inEl->downcast<NativeG>())
        {
            return g;
        }
        else
        {
            return findNativeGParent(inEl->parent());
        }
    }


    bool NativeG::initStyleControllers()
    {
        setStyleController(CSSFillController::PropertyName(), static_cast<CSSFillController*>(this));
        return Super::initStyleControllers();
    }
    
    
    void NativeG::paint(Gdiplus::Graphics & g)
    {
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            if (Painter * svg = owningElement()->children()[idx]->impl()->downcast<Painter>())
            {
                svg->paint(g);
            }
        }
    }

    
    const RGBColor & NativeG::getCSSFill() const
    {
        if (mFill.isValid())
        {
            return mFill;
        }

        if (NativeG * g = findNativeGParent(parent()))
        {
            return g->getCSSFill();
        }

        return mFill; // default value
    }


    void NativeG::setCSSFill(const RGBColor & inColor)
    {
        mFill = inColor;
    }

    
    NativePolygon::NativePolygon(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualBox(inParent, inAttributesMapping)
    {
    }
        
        
    bool NativePolygon::initAttributeControllers()
    {
        setAttributeController("points", static_cast<PointsController*>(this));
        return Super::initAttributeControllers();
    }


    const Points & NativePolygon::getPoints() const
    {
        return mPoints;
    }


    void NativePolygon::setPoints(const Points & inPoints)
    {
        mPoints = inPoints;
        mNativePoints.clear();
        for (size_t idx = 0; idx != mPoints.size(); ++idx)
        {
            const Point & point = mPoints[idx];
            mNativePoints.push_back(Gdiplus::PointF((Gdiplus::REAL)point.x(), (Gdiplus::REAL)point.y()));
        }
    }

    
    void NativePolygon::paint(Gdiplus::Graphics & g)
    {
        if (!mNativePoints.empty())
        {
            Gdiplus::Color color(Gdiplus::Color::Black);
            NativeG * group = findNativeGParent(this);
            if (group)
            {
                RGBColor fill = group->getCSSFill();
                color = Gdiplus::Color(fill.red(), fill.green(), fill.blue());
            }
            Gdiplus::SolidBrush solidBrush(color);
            g.FillPolygon(&solidBrush, &mNativePoints[0], mNativePoints.size());
        }
    }


    RectImpl::RectImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualBox(inParent, inAttributesMapping),
        mX(0),
        mY(0),
        mWidth(0),
        mHeight(0)
    {
    }


    bool RectImpl::initStyleControllers()
    {
        setStyleController("x", static_cast<CSSXController*>(this));
        setStyleController("y", static_cast<CSSYController*>(this));
        setStyleController("width", static_cast<CSSWidthController*>(this));
        setStyleController("height", static_cast<CSSHeightController*>(this));
        setStyleController("fill", static_cast<CSSFillController*>(this));
        return Super::initStyleControllers();
    }


    void RectImpl::paint(Gdiplus::Graphics & g)
    {
        Gdiplus::Color color(Gdiplus::Color::Black);
        RGBColor fill;
        if (mFill.isValid())
        {
            fill = mFill;
        }
        else
        {
            NativeG * group = findNativeGParent(this);
            if (group)
            {
                fill = group->getCSSFill();
            }
        }
        color = Gdiplus::Color(fill.red(), fill.green(), fill.blue());
        Gdiplus::SolidBrush solidBrush(color);
        g.FillRectangle(&solidBrush, Gdiplus::RectF((Gdiplus::REAL)mX,
                                                    (Gdiplus::REAL)mY,
                                                    (Gdiplus::REAL)mWidth,
                                                    (Gdiplus::REAL)mHeight));
    }


    int RectImpl::getCSSX() const
    {
        return mX;
    }


    void RectImpl::setCSSX(int inX)
    {
        mX = inX;
    }


    int RectImpl::getCSSY() const
    {
        return mY;
    }


    void RectImpl::setCSSY(int inY)
    {
        mY = inY;
    }


    int RectImpl::getCSSWidth() const
    {
        return mWidth;
    }


    void RectImpl::setCSSWidth(int inWidth)
    {
        mWidth = inWidth;
    }


    int RectImpl::getCSSHeight() const
    {
        return mHeight;
    }


    void RectImpl::setCSSHeight(int inHeight)
    {
        mHeight = inHeight;
    }

    
    const RGBColor & RectImpl::getCSSFill() const
    {
        if (mFill.isValid())
        {
            return mFill;
        }

        if (NativeG * g = findNativeGParent(parent()))
        {
            return g->getCSSFill();
        }

        return mFill; // default value
    }

    
    int RectImpl::getWidth() const
    {
        return mWidth;
    }


    void RectImpl::setWidth(int inWidth)
    {
        mWidth = inWidth;
    }


    int RectImpl::getHeight() const
    {
        return mHeight;
    }


    void RectImpl::setHeight(int inHeight)
    {
        mHeight = inHeight;
    }


    void RectImpl::setCSSFill(const RGBColor & inColor)
    {
        mFill = inColor;
    }

    
    NativePath::NativePath(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        VirtualBox(inParent, inAttributesMapping)
    {
    }
        
        
    bool NativePath::initAttributeControllers()
    {
        setAttributeController("d", static_cast<PathInstructionsController*>(this));
        setAttributeController("fill", static_cast<FillController*>(this));
        return Super::initAttributeControllers();
    }


    void NativePath::setFill(const RGBColor & inColor)
    {
        mFill = inColor;
    }


    const RGBColor & NativePath::getFill() const
    {
        return mFill;
    }


    void NativePath::getFloatPoints(const PathInstruction & instruction,
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


    void NativePath::GetAbsolutePositions(const PointFs & inRelativePoints,
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


    void NativePath::GetAbsolutePositions(const PathInstruction & instruction,
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


    void NativePath::GetPointReflection(const PointF & inPoint,
                                        const PointF & inOrigin,
                                        PointF & outReflection)
    {
        float x = inOrigin.x() - inPoint.x();
        float y = inOrigin.y() - inPoint.y();
        outReflection = PointF(x, y);
    }

    
    
    Gdiplus::Color NativePath::getColor()
    {
        if (mFill.isValid())
        {
            return Gdiplus::Color(mFill.getValue().red(), mFill.getValue().green(), mFill.getValue().blue());
        }
        else
        {
            NativeG * group = findNativeGParent(this);
            if (group)
            {
                RGBColor fill = group->getCSSFill();
                return Gdiplus::Color(fill.red(), fill.green(), fill.blue());
            }
        }        
        return Gdiplus::Color(Gdiplus::Color::Black);
    }

    
    void NativePath::GetPreparedInstructions(const PathInstructions & inData, PathInstructions & outPrepData)
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
                    GetAbsolutePositions(instruction, prevPoint, preppedPoints);
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
                        GetPointReflection(prevInstruction.getPoint(1),
										   prevInstruction.getPoint(2),
										   c1);
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
                        GetPointReflection(prevInstruction.getPoint(0),
										   prevInstruction.getPoint(1),
										   c1);
						curveInstruction.points().insert(curveInstruction.points().begin(), c1);
                    }
					else
					{
						// If the S command doesn't follow another S or C command, then it is
						// assumed that both control points for the curve are the same.
						curveInstruction.points().insert(curveInstruction.points().begin(),
														 instruction.getPoint(0));
					}
                    GetAbsolutePositions(curveInstruction, prevPoint, preppedPoints);
                    if (!preppedPoints.empty())
                    {
                        prevPoint = preppedPoints[preppedPoints.size() - 1];
                    }
                    break;
                }
                //PathInstruction::QuadraticBelzierCurve,          // Q
                //PathInstruction::SmoothQuadraticBelzierCurveTo,  // T
                //PathInstruction::EllipticalArc,                  // A
                case PathInstruction::ClosePath: // Z
                {
                    if (!preppedPoints.empty())
                    {
                        outPrepData.push_back(PathInstruction(prevInstruction.type(),
                                                              PathInstruction::Absolute,
                                                              preppedPoints));
                        preppedPoints.clear();
                    }
                    else
                    {
                        ReportError("SVG ClosePath: nothing found to close.");
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

    
    void NativePath::paint(Gdiplus::Graphics & g)
    {
        Gdiplus::Color color(getColor());
        Gdiplus::SolidBrush solidBrush(color);
        Gdiplus::Pen pen(color, 1);
        for (size_t idx = 0; idx != mPreparedInstructions.size(); ++idx)
        {
            const PathInstruction & instruction = mPreparedInstructions[idx];
            const PointFs & points = instruction.points();
            switch (instruction.type())
            {
                case PathInstruction::MoveTo:
                {
                    assert(false); // should not be included in prepped instructions
                    break;
                }
                case PathInstruction::LineTo:
                case PathInstruction::HorizontalLineTo:
                case PathInstruction::VerticalLineTo:
                {
                    if (instruction.numPoints() == 2)
                    {
                        g.DrawLine(&pen,
                                   instruction.getPoint(0).x(),
                                   instruction.getPoint(0).y(),
                                   instruction.getPoint(1).x(),
                                   instruction.getPoint(1).y());
                    }
                    else
                    {
                        ReportError("LineTo: paint failed because num prepped points is not equal to 2.");
                    }
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
                    Gdiplus::GraphicsPath bezierPath;
                    bezierPath.AddBeziers(&gdiplusPoints[0], gdiplusPoints.size());
                    g.FillPath(&solidBrush, &bezierPath);
                    break;
                }
                case PathInstruction::QuadraticBelzierCurve:
                case PathInstruction::SmoothQuadraticBelzierCurveTo:
                case PathInstruction::EllipticalArc:
                {
                    assert(false); // not yet implemented
                    break;
                }
                case PathInstruction::ClosePath:
                {
                    assert(false); // should not be included in prepped
                    break;
                }
                default:
                {
                    assert(false);
                    ReportError("Unsupported type");
                }
            }
        }
    }
    
    
    const PathInstructions & NativePath::getPathInstructions() const
    {
        return mInstructions;
    }


    void NativePath::setPathInstructions(const PathInstructions & inPathInstructions)
    {
        mInstructions = inPathInstructions;
        mPreparedInstructions.clear();
        GetPreparedInstructions(mInstructions, mPreparedInstructions);
    }


} // namespace SVG

} // namespace XULWin
