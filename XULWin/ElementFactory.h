#ifndef ELEMENTFACTOR_H_INCLUDED
#define ELEMENTFACTOR_H_INCLUDED


#include "Element.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>


namespace XULWin
{
            
   
    class ElementFactory
    {
    public:

        static ElementFactory & Instance();

        ElementPtr createElement(const std::string & inType, Element * inParent, const AttributesMapping & inAttr);

        template<class ElementType>
        void registerElement()
        {
            mFactoryMethods.insert(std::make_pair(ElementType::Type(),
                                   boost::bind(ElementType::Create, _1, _2)));
        }
    
    private:
        ElementFactory();
        typedef boost::function<ElementPtr(Element*, const AttributesMapping &)> FactoryMethod;
        typedef std::map<std::string, FactoryMethod> FactoryMethods;
        FactoryMethods mFactoryMethods;
    };

} // namespace XULWin


#endif // ELEMENTFACTOR_H_INCLUDED
