#include "WinXUL/WinXUL.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <map>
#include <string>


namespace WinXUL
{

    template<class T>
    Component * CreateComponent(Component * inParent)
    {
        return new T(inParent);
    }

    typedef boost::function<Component*(Component*)> FactoryFunction;

    std::map<std::string, FactoryFunction> gLookupTable;

    template<class T>
    void RegisterElement(const std::string & inTagName)
    {
        gLookupTable.insert(std::make_pair("label", boost::bind(&CreateComponent<Label>, _1)));
    }

} // namespace WinXUL


using namespace WinXUL;


int main()
{    
    RegisterElement<Label>("label");
    return 0;
}
