#include "Element.h"
#include "ElementFactory.h"
#include "NativeComponent.h"
#include "Parser.h"
#include "Utils/ErrorReporter.h"
#include <boost/scoped_ptr.hpp>
#include <windows.h>


using namespace XULWin;
using namespace Utils;


void registerTypes(HMODULE inModule)
{
    NativeWindow::Register(inModule);
    ElementFactory::Instance().registerElement<Window>("window");
    ElementFactory::Instance().registerElement<Button>("button");
    ElementFactory::Instance().registerElement<CheckBox>("checkbox");
    ElementFactory::Instance().registerElement<Label>("label");
    ElementFactory::Instance().registerElement<HBox>("hbox");
    ElementFactory::Instance().registerElement<VBox>("vbox");
    ElementFactory::Instance().registerElement<Box>("box");
    ElementFactory::Instance().registerElement<MenuList>("menulist");
    ElementFactory::Instance().registerElement<MenuPopup>("menupopup");
    ElementFactory::Instance().registerElement<MenuItem>("menuitem");
    ElementFactory::Instance().registerElement<TextBox>("textbox");
}


class TestDropDown
{
public:
    void run()
    {
    	mParser.parse("Dropdown.xul");
        if (mParser.rootElement())
        {
            ElementPtr addButton = mParser.rootElement()->getElementById("addbutton");
            if (addButton)
            {
                addButton->addEventListener("command", boost::bind(&TestDropDown::addButtonPressed, this, _1));
            }

            ElementPtr removeButton = mParser.rootElement()->getElementById("removebutton");
            if (removeButton)
            {
                removeButton->addEventListener("command", boost::bind(&TestDropDown::removeButtonPressed, this, _1));
            }
            static_cast<Window*>(mParser.rootElement().get())->showModal();
        }
    }


    void addButtonPressed(Event * inEvent)
    {
        ElementPtr popup = mParser.rootElement()->getElementById("popup");
        ElementPtr input = mParser.rootElement()->getElementById("input");
        if (popup && input)
        {
            AttributesMapping attr;
            attr["label"] = input->getAttribute("value");
            MenuItem::Create(popup, attr);
        }
    }


    void removeButtonPressed(Event * inEvent)
    {
    }

private:
    Parser mParser;
};


void runSample()
{
	Parser parser;
    parser.parse("test.xul");
    static_cast<Window*>(parser.rootElement().get())->showModal();
}


void log(const std::string & inMessage)
{
    MessageBoxA(0, inMessage.c_str(), "XULWin Logger", MB_OK);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    ErrorReporter::Initialize();
    ErrorReporter::Instance().setLogger(boost::bind(&log, _1));
    registerTypes(hInstance);
    {
        TestDropDown t;
        t.run();
    }
    ErrorReporter::Finalize();
    return 0;
}
