#include "Element.h"
#include "NativeComponent.h"
#include <windows.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>


using namespace XULWin;

typedef boost::function<ElementPtr(ElementPtr, elid)> FactoryMethod;
typedef std::map<eltype, FactoryMethod> FactoryMethods;
FactoryMethods gFactoryMethods;


template<class ElementType>
void mapXUL(const eltype & inType)
{
    gFactoryMethods.insert(std::make_pair(inType, boost::bind(ElementType::Create, _1, _2)));
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
    ElementPtr window(Window::Create(noParent, elid("Test")));
    ElementPtr hbox(HBox::Create(window, elid("hbox1")));

    ElementPtr check(CheckBox::Create(hbox, elid("chk1")));
    check->Attributes["flex"] = "1";

    ElementPtr button1(Button::Create(hbox, elid("btn1")));
    button1->Attributes["flex"] = "2";

    ElementPtr button2(Button::Create(hbox, elid("btn2")));
    button2->Attributes["flex"] = "3";

    ElementPtr check2(CheckBox::Create(hbox, elid("chk2")));
    check2->Attributes["flex"] = "4";
    static_cast<Window*>(window.get())->showModal();
    return 0;
}
