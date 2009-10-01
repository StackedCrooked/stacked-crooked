#include "Initializer.h"
#include "XULWin/ElementImpl.h"
#include "XULWin/Image.h"
#include "XULWin/Script.h"
#include "Utils/ErrorReporter.h"


using namespace Utils;


namespace XULWin
{
    
    Initializer::Initializer(HINSTANCE inModuleHandle)
    {
        ErrorReporter::Initialize();
        NativeWindow::Register(inModuleHandle);
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
        ElementFactory::Instance().registerElement<Scrollbar>();
        ElementFactory::Instance().registerElement<Script>();
        ElementFactory::Instance().registerElement<TabBox>();
        ElementFactory::Instance().registerElement<Tabs>();
        ElementFactory::Instance().registerElement<Tab>();
        ElementFactory::Instance().registerElement<TabPanels>();
        ElementFactory::Instance().registerElement<TabPanel>();
    }


    Initializer::~Initializer()
    {
        ErrorReporter::Finalize();
    }

} // namespace XULWin
