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

        ElementPtr createElement(const std::string & inType, ElementPtr inParent, const AttributesMapping & inAttr);

        template<class ElementType>
        void registerElement(const std::string & inType)
        {
            mFactoryMethods.insert(
                std::make_pair(inType, boost::bind(ElementType::Create, _1, _2, _3)));
        }
    
    private:
        ElementFactory();
        typedef boost::function<ElementPtr(std::string, ElementPtr, const AttributesMapping &)> FactoryMethod;
        typedef std::map<std::string, FactoryMethod> FactoryMethods;
        FactoryMethods mFactoryMethods;
    };


} // namespace XULWin


#endif // ELEMENTFACTOR_H_INCLUDED
