#include "Element.h"
#include "NativeComponent.h"
#include "ErrorHandler/ErrorStack.h"
#include <windows.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>


using namespace XULWin;
using namespace CppToys;


typedef boost::function<ElementPtr(eltype, ElementPtr)> FactoryMethod;
typedef std::map<eltype, FactoryMethod> FactoryMethods;
FactoryMethods gFactoryMethods;


template<class ElementType>
void mapXUL(const eltype & inType)
{
    gFactoryMethods.insert(std::make_pair(inType, boost::bind(ElementType::Create, _1, _2)));
}

ElementPtr createElement(const eltype & inType, ElementPtr inParent)
{
    ElementPtr result;
    FactoryMethods::iterator it = gFactoryMethods.find(inType);
    if (it != gFactoryMethods.end())
    {
        result = it->second(inType, inParent);
    }
    else
    {
        ThrowError("No mapping found for XUL type " + std::string(inType));
    }
    return result;
}


void registerTypes(HMODULE inModule)
{
    NativeWindow::Register(inModule);
    mapXUL<Window>(eltype("window"));
    mapXUL<Button>(eltype("button"));
    mapXUL<CheckBox>(eltype("checkbox"));
    mapXUL<HBox>(eltype("hbox"));
}


void runSample()
{
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    registerTypes(hInstance);
   
    ElementPtr noParent;
    ElementPtr window = createElement(eltype("window"), noParent);
    ElementPtr hbox = createElement(eltype("hbox"), window);

    ElementPtr check = createElement(eltype("checkbox"), hbox);
    check->Attributes["flex"] = "1";

    ElementPtr button1 = createElement(eltype("button"), hbox);
    button1->Attributes["flex"] = "2";

    static_cast<Window*>(window.get())->showModal();
    return 0;
}
