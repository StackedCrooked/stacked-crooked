#include "Element.h"
#include "ElementFactory.h"
#include "NativeComponent.h"
#include "Parser.h"
#include "Utils/ErrorReporter.h"
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


void addButtonPressed(Event * inEvent)
{
}


void removeButtonPressed(Event * inEvent)
{
}


void runDropdownSample()
{
    ErrorCatcher errorCatcher;
	Parser parser;
	parser.setContentHandler(&parser);
	
	try
	{
		parser.parse("Dropdown.xul");
	}
	catch (Poco::Exception& e)
	{
		ReportError(e.displayText());
        return;
	}
    if (errorCatcher.hasCaught())
    {
        errorCatcher.log();
    }
    ElementPtr addButton = parser.rootElement()->getElementById("addbutton");
    addButton->addEventListener("command", boost::bind(addButtonPressed, _1));

    ElementPtr removeButton = parser.rootElement()->getElementById("removebutton");
    removeButton->addEventListener("command", boost::bind(removeButtonPressed, _1));
    static_cast<Window*>(parser.rootElement().get())->showModal();
}


void runSample()
{
    ErrorCatcher errorCatcher;
	Parser parser;
	//parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACES, true);
	//parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACE_PREFIXES, true);
	parser.setContentHandler(&parser);
	
	try
	{
		parser.parse("test.xul");
	}
	catch (Poco::Exception& e)
	{
		ReportError(e.displayText());
        return;
	}
    if (errorCatcher.hasCaught())
    {
        errorCatcher.log();
    }
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
    runDropdownSample();
    ErrorReporter::Finalize();
    return 0;
}
