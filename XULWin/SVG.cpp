#include "SVG.h"
#include "Graphics.h"
#include "Image.h"
#include "AttributeController.h"
#include "StyleController.h"
#include "Decorator.h"
#include "ElementImpl.h"
#include "Defaults.h"
#include "Utils/Fallible.h"
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
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
        virtual void erase(Gdiplus::Graphics & g);

        virtual void paint(HDC inHDC);
    };

    
    NativeSVG::NativeSVG(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent, inAttributesMapping, TEXT("STATIC"), 0, WS_BORDER)
    {
    }
        
        
    int NativeSVG::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        return 480;
    }

    
    int NativeSVG::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        return 480;
    }
    
    
    void NativeSVG::erase(Gdiplus::Graphics & g)
    {
        Gdiplus::SolidBrush brush(Gdiplus::Color::White);
        Rect r(clientRect());
        Gdiplus::RectF rectF((Gdiplus::REAL)r.x(),
                             (Gdiplus::REAL)r.y(),
                             (Gdiplus::REAL)r.width(),
                             (Gdiplus::REAL)r.height());
        g.FillRectangle(&brush, rectF);
    }
    
    
    void NativeSVG::paint(HDC inHDC)
    {
        Gdiplus::Graphics g(inHDC);
        erase(g);
        for (size_t idx = 0; idx != owningElement()->children().size(); ++idx)
        {
            if (Painter * svg = owningElement()->children()[idx]->impl()->downcast<Painter>())
            {
                svg->paint(g);
            }
        }
    }

    
    LRESULT NativeSVG::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        if (inMessage == WM_PAINT)
        {
            HDC hDC = ::GetDC(handle());
            PAINTSTRUCT ps;
            ps.hdc = hDC;
            ::BeginPaint(handle(), &ps);
            paint(hDC);
            ::EndPaint(handle(), &ps);
            ::ReleaseDC(handle(), hDC);
            return TRUE;
        }
        return Super::handleMessage(inMessage, wParam, lParam);
    }


    class NativeG : public PassiveComponent,
                    public Painter,
                    public CSSFillController
    {
    public:
        typedef PassiveComponent Super;

        NativeG(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initStyleControllers();

        virtual void setCSSFill(const RGBColor & inColor);

        virtual const RGBColor & getCSSFill() const;

        virtual void paint(Gdiplus::Graphics & g);

    private:
        Utils::Fallible<RGBColor> mFill;
    };


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


    SVG::SVG(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(SVG::Type(),
                inParent,
                new NativeSVG(inParent->impl(), inAttributesMapping))
    {
    }

        
    G::G(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(G::Type(),
                inParent,
                new NativeG(inParent->impl(), inAttributesMapping))
    {
    }


    Polygon::Polygon(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Polygon::Type(),
                inParent,
                new NativePolygon(inParent->impl(), inAttributesMapping))
    {
    }

} // namespace SVG

} // namespace XULWin