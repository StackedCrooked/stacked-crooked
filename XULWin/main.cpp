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
}


void runSample()
{

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
		ThrowError(e.displayText());
        return;
	}
    static_cast<Window*>(parser.rootElement().get())->showModal();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    ErrorReporter::Initialize();
    registerTypes(hInstance);
    runSample();
    ErrorReporter::Finalize();
    return 0;
}
