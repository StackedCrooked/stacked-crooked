#ifndef ELEMENTCREATIONSUPPORT_H_INCLUDED
#define ELEMENTCREATIONSUPPORT_H_INCLUDED


#include "ElementImpl.h"
#include "ToolbarCustomWindowDecorator.h"
#include "Poco/StringTokenizer.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>


namespace XULWin
{    

    template<class ControlType>
    static ElementImpl * CreateNativeControl(Element * inParent, const AttributesMapping & inAttributesMapping)
    {
        if (ToolbarImpl * toolbar = inParent->impl()->downcast<ToolbarImpl>())
        {
            ControlType * control = new ControlType(inParent->impl(), inAttributesMapping);
            boost::weak_ptr<Utils::Toolbar> weakToolbar(toolbar->nativeToolbar());
            return new ToolbarCustomWindowDecorator(control, weakToolbar);
        }
        else
        {
            return new ControlType(inParent->impl(), inAttributesMapping);
        }
    }


    static void GetStyles(const AttributesMapping & inAttributesMapping, StylesMapping & styles)
    {        
        StylesMapping::const_iterator it = inAttributesMapping.find("style");
        if (it != inAttributesMapping.end())
        {
            Poco::StringTokenizer tok(it->second, ";:", Poco::StringTokenizer::TOK_IGNORE_EMPTY | Poco::StringTokenizer::TOK_TRIM);
            Poco::StringTokenizer::Iterator it = tok.begin(), end = tok.end();
            std::string key, value;
            int counter = 0;
            for (; it != end; ++it)
            {
                if (counter%2 == 0)
                {
                    key = *it;
                }
                else
                {
                    value = *it;
                    styles.insert(std::make_pair(key, value));
                }
                counter++;
            }
        }
    }


    static CSSOverflow GetOverflow(const StylesMapping & inStyles,
                                   const std::string & inOverflow)
    {
        StylesMapping::const_iterator it = inStyles.find(inOverflow);
        if (it == inStyles.end())
        {
            it = inStyles.find("overflow");
        }
        if (it != inStyles.end())
        {
            return String2CSSOverflow(it->second, CSSOverflow_Visible);
        }
        return CSSOverflow_Hidden;
    }


    template<class VirtualType, class NativeType>
    static ElementImpl * CreateContainer(Element * inParent, const AttributesMapping & inAttributesMapping)
    {
        StylesMapping styles;
        GetStyles(inAttributesMapping, styles);
        CSSOverflow overflowX = GetOverflow(styles, "overflow-x");
        CSSOverflow overflowY = GetOverflow(styles, "overflow-y");
        if (overflowX != CSSOverflow_Hidden || overflowY != CSSOverflow_Hidden)
        {
            ElementImpl * box = CreateNativeControl<NativeType>(inParent, inAttributesMapping);
            return new ScrollDecorator(inParent->impl(), box, overflowX, overflowY);
        }
        else
        {
            return new Decorator(CreateNativeControl<VirtualType>(inParent, inAttributesMapping));
        }
    }
}


#endif //  ELEMENTCREATIONSUPPORT_H_INCLUDED
