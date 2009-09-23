#include "Element.h"
#include "ElementImpl.h"
#include "ElementFactory.h"
#include "Image.h"
#include "Parser.h"
#include "Utils/ErrorReporter.h"
#include "Poco/StringTokenizer.h"
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
    ElementFactory::Instance().registerElement<RadioGroup>();
    ElementFactory::Instance().registerElement<Radio>();
    ElementFactory::Instance().registerElement<ProgressMeter>();
    ElementFactory::Instance().registerElement<Deck>();
    ElementFactory::Instance().registerElement<Image>();
}


class TestDropDown
{
public:
    void run()
    {
    	mParser.parse("chrome/content/Dropdown.xul");
        if (mParser.rootElement())
        {
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


void runXUL(const std::string & inFileName)
{
	Parser parser;
    parser.parse(inFileName);
    if (Window * window = parser.rootElement()->downcast<Window>())
    {
        window->showModal();
    }
}


void runDropDownSample()
{
    TestDropDown t;
    t.run();
}


void runNoXULSample()
{
    AttributesMapping attr;
    ElementPtr window = Window::Create(0, attr);
    ElementPtr vbox = VBox::Create(window.get(), attr);

    ElementPtr hbox1 = HBox::Create(vbox.get(), attr);

    attr["value"] = "Username:";
    ElementPtr label = Label::Create(hbox1.get(), attr);
    attr.clear();

    attr["flex"] = "1";
    ElementPtr text = TextBox::Create(hbox1.get(), attr);
    attr.clear();

    ElementPtr hbox2 = HBox::Create(vbox.get(), attr);

    attr["value"] = "Password:";
    ElementPtr passLabel = Label::Create(hbox2.get(), attr);
    attr.clear();

    attr["flex"] = "1";
    ElementPtr passText = TextBox::Create(hbox2.get(), attr);
    attr.clear();

    window->downcast<Window>()->showModal();
    
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
    runXUL("Shout.xul");
    //runDropDownSample();
    //runNoXULSample();



    ErrorReporter::Finalize();
    return 0;
}
