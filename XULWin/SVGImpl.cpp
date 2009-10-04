#include "SVGImpl.h"
#include "PathInstructions.h"
#include "Utils/ErrorReporter.h"


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


    //void NativePath::getEndPoint(const PathInstruction & instruction,
    //                             const Gdiplus::PointF & inPrevPoint,
    //                             Gdiplus::PointF & outEndPoint)
    //{
    //    if (instruction.numPoints() == 0)
    //    {
    //        assert(false);
    //        return;
    //    }

    //    Point p = instruction.getPoint(0);
    //    if (instruction.positioning() == PathInstruction::Absolute)
    //    {
    //        outEndPoint = Gdiplus::PointF((Gdiplus::REAL)p.x(),
    //                                      (Gdiplus::REAL)p.y());
    //    }
    //    else
    //    {
    //        outEndPoint = Gdiplus::PointF(inPrevPoint.X + (Gdiplus::REAL)p.x(),
    //                                      inPrevPoint.Y + (Gdiplus::REAL)p.y());
    //    }
    //}


    void NativePath::getFloatPoints(const PathInstruction & instruction,
                                    const Gdiplus::PointF & inPrevPoint,
                                    std::vector<Gdiplus::PointF> & outPoints)
    {
        Gdiplus::PointF pointF = inPrevPoint;
        for (size_t idx = 0; idx != instruction.numPoints(); ++idx)
        {
            const Point & point = instruction.getPoint(idx);
            if (instruction.positioning() == PathInstruction::Absolute)
            {
                pointF = Gdiplus::PointF((Gdiplus::REAL)point.x(),
                                         (Gdiplus::REAL)point.y());
            }
            else
            {
                pointF = Gdiplus::PointF(pointF.X + (Gdiplus::REAL)point.x(),
                                         pointF.Y + (Gdiplus::REAL)point.y());
            }
            outPoints.push_back(pointF);
        }
    }

    
    void NativePath::paint(Gdiplus::Graphics & g)
    {
        Gdiplus::Color color(Gdiplus::Color::Black);
        if (mFill.isValid())
        {
            color = Gdiplus::Color(mFill.getValue().red(), mFill.getValue().green(), mFill.getValue().blue());
        }
        else
        {
            NativeG * group = findNativeGParent(this);
            if (group)
            {
                RGBColor fill = group->getCSSFill();
                color = Gdiplus::Color(fill.red(), fill.green(), fill.blue());
            }
        }
        Gdiplus::SolidBrush solidBrush(color);
        Gdiplus::Pen pen(color, 2);

        std::vector<Gdiplus::PointF> prevPoints;
        const PathInstruction * prevInstruction = 0;
        Gdiplus::PointF prevPointF;
        for (size_t idx = 0; idx != mInstructions.size(); ++idx)
        {
            const PathInstruction & instruction = mInstructions[idx];
            switch (instruction.type())
            {
                case PathInstruction::MoveTo:
                {
                    assert(instruction.numPoints() > 0);
                    if (instruction.numPoints() > 0)
                    {
                        Point p = instruction.getPoint(0);
                        if (instruction.positioning() == PathInstruction::Absolute)
                        {
                            prevPointF = Gdiplus::PointF((float)p.x(), (float)p.y());
                        }
                        else
                        {
                            prevPointF.X += (float)p.x();
                            prevPointF.Y += (float)p.y();
                        }
                    }
                    break;
                }
                case PathInstruction::LineTo:
                {
                    assert(instruction.numPoints() > 0);
                    if (instruction.numPoints() > 0)
                    {
                        std::vector<Gdiplus::PointF> points;
                        getFloatPoints(instruction, prevPointF, points);
                        if (points.size() == 2)
                        {
                            g.DrawLine(&pen, points[0], points[1]);
                            prevPointF = points[1];
                        }
                    }
                    break;
                }
                case PathInstruction::HorizontalLineTo:
                {
                    assert(false);
                    // don't know what to do here
                    break;
                }
                case PathInstruction::VerticalLineTo:
                {
                    assert(false);
                    // don't know what to do here
                    break;
                }
                case PathInstruction::CurveTo:
                {
                    assert(instruction.numPoints() > 0);
                    if (instruction.numPoints() > 0)
                    {
                        std::vector<Gdiplus::PointF> points;
                        points.push_back(prevPointF);
                        getFloatPoints(instruction, prevPointF, points);
                        assert(points.size() == 4);
                        g.DrawBeziers(&pen, &points[0], points.size());
                        if (!points.empty())
                        {
                            prevPointF = points[points.size() - 1];
                        }
                        // remember for next iteration, we may need it in SmoothCurveTo case
                        prevPoints = points; 
                    }
                    break;
                }
                case PathInstruction::SmoothCurveTo:
                {
                    assert(instruction.numPoints() > 0);
                    if (instruction.numPoints() > 0)
                    {
                        std::vector<Gdiplus::PointF> points;
                        getFloatPoints(instruction, prevPointF, points);
                        assert(points.size() == 2);
                        if (points.size() == 2)
                        {
                            if (prevInstruction &&
                                (prevInstruction->type() == PathInstruction::CurveTo
                                 || prevInstruction->type() == PathInstruction::SmoothCurveTo)
                                )
                            {
                                std::vector<Gdiplus::PointF> backup = points;
                                points.clear();
                                points.push_back(prevPointF);
                                points.push_back(prevPoints[1]);
                                points.push_back(backup[0]);
                                points.push_back(backup[1]);
                            }
                            else
                            {
                                std::vector<Gdiplus::PointF> backup = points;
                                points.clear();
                                points.push_back(prevPointF);
                                points.push_back(backup[0]);
                                points.push_back(backup[0]);
                                points.push_back(backup[1]);
                            }
                        }
                        assert(points.size() == 4);
                        g.DrawBeziers(&pen, &points[0], points.size());
                        if (!points.empty())
                        {
                            prevPointF = points[points.size() - 1];
                        }
                        prevPoints = points;
                    }
                    break;
                }
                case PathInstruction::QuadraticBelzierCurve:
                {
                    assert(false);
                    if (instruction.numPoints() > 0)
                    {
                        std::vector<Gdiplus::PointF> points;
                        getFloatPoints(instruction, prevPointF, points);
                        g.FillClosedCurve(&solidBrush, &points[0], points.size());
                    }
                    break;
                }
                case PathInstruction::SmoothQuadraticBelzierCurveTo:
                {
                    assert(false);
                    if (instruction.numPoints() > 0)
                    {
                        std::vector<Gdiplus::PointF> points;
                        getFloatPoints(instruction, prevPointF, points);
                        g.FillClosedCurve(&solidBrush, &points[0], points.size());
                    }
                    break;
                }
                case PathInstruction::EllipticalArc:
                {
                    assert(false);
                    //g.DrawArc(
                    break;
                }
                case PathInstruction::ClosePath:
                {
                    assert(false);
                    // TODO: implement
                    break;
                }
                default:
                {
                    assert(false);
                    Utils::ReportError("Unsupported type");
                }
            }
            prevInstruction = &mInstructions[idx];
        }
    }
    
    
    const PathInstructions & NativePath::getPathInstructions() const
    {
        return mInstructions;
    }


    void NativePath::setPathInstructions(const PathInstructions & inPathInstructions)
    {
        mInstructions = inPathInstructions;
    }


} // namespace SVG

} // namespace XULWin
