#include "ElementFactory.h"
#include "ErrorHandler/ErrorStack.h"


using namespace CppToys;


namespace XULWin
{

    ElementFactory & ElementFactory::Instance()
    {
        static ElementFactory fInstance;
        return fInstance;
    }


    ElementFactory::ElementFactory()
    {
    }


    ElementPtr ElementFactory::createElement(const eltype & inType, ElementPtr inParent)
    {
        ElementPtr result;
        FactoryMethods::iterator it = mFactoryMethods.find(inType);
        if (it != mFactoryMethods.end())
        {
            result = it->second(inType, inParent);
            result->init();
        }
        else
        {
            ThrowError("No mapping found for XUL type " + std::string(inType));
        }
        return result;
    }

} // namespace XULWin
