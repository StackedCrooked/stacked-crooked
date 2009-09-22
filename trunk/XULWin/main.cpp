#include "Element.h"
#include "ElementFactory.h"
#include "NativeComponent.h"
#include "Parser.h"
#include "Utils/ErrorReporter.h"
#include <boost/scoped_ptr.hpp>
#include <windows.h>
#include <commctrl.h>


using namespace XULWin;
using namespace Utils;


void registerTypes(HMODULE inModule)
{
    NativeWindow::Register(inModule);
    ElementFactory::Instance().registerElement<Window>();
    ElementFactory::Instance().registerElement<Button>();
    ElementFactory::Instance().registerElement<CheckBox>();
    ElementFactory::Instance().registerElement<Label>();
    ElementFactory::Instance().registerElement<Text>();
    ElementFactory::Instance().registerElement<HBox>();
    ElementFactory::Instance().registerElement<VBox>();
    ElementFactory::Instance().registerElement<Box>();
    ElementFactory::Instance().registerElement<MenuList>();
    ElementFactory::Instance().registerElement<MenuPopup>();
    ElementFactory::Instance().registerElement<MenuItem>();
    ElementFactory::Instance().registerElement<TextBox>();
    ElementFactory::Instance().registerElement<Separator>();
    ElementFactory::Instance().registerElement<MenuButton>();
    ElementFactory::Instance().registerElement<Grid>();
    ElementFactory::Instance().registerElement<Rows>();
    ElementFactory::Instance().registerElement<Row>();
    ElementFactory::Instance().registerElement<Columns>();
    ElementFactory::Instance().registerElement<Column>();
    ElementFactory::Instance().registerElement<Description>();
    ElementFactory::Instance().registerElement<Spacer>();
}


class TestDropDown
{
    std::vector<boost::signals::connection> mConnections;
public:
    void run()
    {
    	mParser.parse("Dropdown.xul");
        if (mParser.rootElement())
        {
            Element * addButton = mParser.rootElement()->getElementById("addbutton");
            if (addButton)
            {
                //mConnections.push_back(addButton->OnCommand.connect(boost::bind(&TestDropDown::addButtonPressed, this, _1)));
            }

            Element * removeButton = mParser.rootElement()->getElementById("removebutton");
            if (removeButton)
            {
                //mConnections.push_back(removeButton->OnCommand.connect(boost::bind(&TestDropDown::removeButtonPressed, this, _1)));
            }

            Window * window = mParser.rootElement()->downcast<Window>();
            if (window)
            {
                window->showModal();
            }
        }
    }


    void addButtonPressed(Event * inEvent)
    {
        Element * popup = mParser.rootElement()->getElementById("popup");
        Element * input = mParser.rootElement()->getElementById("input");
        if (popup && input)
        {
            AttributesMapping attr;
            std::string value = input->getAttribute("value");
            attr["label"] = value;
            MenuItem::Create(popup, attr);
        }
    }


    void removeButtonPressed(Event * inEvent)
    {
    }

private:
    Parser mParser;
};


void runTestSample()
{
	Parser parser;
    parser.parse("test.xul");
    static_cast<Window*>(parser.rootElement().get())->showModal();
}


void runDropDownSample()
{
    TestDropDown t;
    t.run();
}


void log(const std::string & inMessage)
{
    //MessageBoxA(0, inMessage.c_str(), "XULWin Logger", MB_OK);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Ensure that the common control DLL is loaded. 
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_WIN95_CLASSES
					| ICC_DATE_CLASSES
					| ICC_USEREX_CLASSES
					| ICC_COOL_CLASSES
					| ICC_BAR_CLASSES;;
	BOOL ok = InitCommonControlsEx(&icex);
    assert(ok);

    ErrorReporter::Initialize();
    ErrorReporter::Instance().setLogger(boost::bind(&log, _1));
    registerTypes(hInstance);
    //runTestSample();
    runDropDownSample();
    ErrorReporter::Finalize();
    return 0;
}
