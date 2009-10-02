#include "SVG.h"
#include "SVGImpl.h"
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


    Rect::Rect(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Rect::Type(),
                inParent,
                new RectImpl(inParent->impl(), inAttributesMapping))
    {
    }



} // namespace SVG

} // namespace XULWin