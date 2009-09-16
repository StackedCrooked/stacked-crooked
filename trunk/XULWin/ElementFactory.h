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

        ElementPtr createElement(const eltype & inType, ElementPtr inParent);

        template<class ElementType>
        void registerElement(const eltype & inType)
        {
            mFactoryMethods.insert(
                std::make_pair(inType, boost::bind(ElementType::Create, _1, _2)));
        }
    
    private:
        ElementFactory();
        typedef boost::function<ElementPtr(eltype, ElementPtr)> FactoryMethod;
        typedef std::map<eltype, FactoryMethod> FactoryMethods;
        FactoryMethods mFactoryMethods;
    };


} // namespace XULWin


#endif // ELEMENTFACTOR_H_INCLUDED
