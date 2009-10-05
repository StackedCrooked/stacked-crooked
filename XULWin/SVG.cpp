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
                new SVGCanvas(inParent->impl(), inAttributesMapping))
    {
    }

        
    Group::Group(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Group::Type(),
                inParent,
                new SVGGroupImpl(inParent->impl(), inAttributesMapping))
    {
    }


    Polygon::Polygon(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Polygon::Type(),
                inParent,
                new SVGPolygonImpl(inParent->impl(), inAttributesMapping))
    {
    }


    Rect::Rect(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Rect::Type(),
                inParent,
                new SVGRectImpl(inParent->impl(), inAttributesMapping))
    {
    }


    Path::Path(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Path::Type(),
                inParent,
                new SVGPathImpl(inParent->impl(), inAttributesMapping))
    {
    }



} // namespace SVG

} // namespace XULWin