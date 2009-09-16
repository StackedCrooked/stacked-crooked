#include "Element.h"
#include "ElementFactory.h"
#include "NativeComponent.h"
#include "ErrorHandler/ErrorStack.h"
#include <windows.h>


using namespace XULWin;
using namespace CppToys;



void registerTypes(HMODULE inModule)
{
    NativeWindow::Register(inModule);
    ElementFactory::Instance().registerElement<Window>(eltype("window"));
    ElementFactory::Instance().registerElement<Button>(eltype("button"));
    ElementFactory::Instance().registerElement<CheckBox>(eltype("checkbox"));
    ElementFactory::Instance().registerElement<HBox>(eltype("hbox"));
}


void runSample()
{
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    registerTypes(hInstance);
   
    ElementPtr noParent;
    ElementPtr window = ElementFactory::Instance().createElement(eltype("window"), noParent);
    ElementPtr hbox = ElementFactory::Instance().createElement(eltype("hbox"), window);

    ElementPtr check = ElementFactory::Instance().createElement(eltype("checkbox"), hbox);
    check->Attributes["flex"] = "1";

    ElementPtr button1 = ElementFactory::Instance().createElement(eltype("button"), hbox);
    button1->Attributes["flex"] = "2";

    static_cast<Window*>(window.get())->showModal();
    return 0;
}
