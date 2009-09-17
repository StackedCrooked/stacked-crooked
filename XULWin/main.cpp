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
    ElementFactory::Instance().registerElement<Window>(eltype("window"));
    ElementFactory::Instance().registerElement<Button>(eltype("button"));
    ElementFactory::Instance().registerElement<CheckBox>(eltype("checkbox"));
    ElementFactory::Instance().registerElement<Label>(eltype("label"));
    ElementFactory::Instance().registerElement<HBox>(eltype("hbox"));
    ElementFactory::Instance().registerElement<VBox>(eltype("vbox"));
    ElementFactory::Instance().registerElement<Box>(eltype("box"));
    ElementFactory::Instance().registerElement<MenuList>(eltype("menulist"));
    ElementFactory::Instance().registerElement<MenuPopup>(eltype("menupopup"));
    ElementFactory::Instance().registerElement<MenuItem>(eltype("menuitem"));
    ElementFactory::Instance().registerElement<TextBox>(eltype("textbox"));
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
