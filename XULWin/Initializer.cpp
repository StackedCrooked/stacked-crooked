#include "Initializer.h"
#include "XULWin/ElementImpl.h"
#include "XULWin/Image.h"
#include "XULWin/Script.h"
#include "XULWin/SVG.h"
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
        ElementFactory::Instance().registerElement<GroupBox>();
        ElementFactory::Instance().registerElement<Caption>();
        ElementFactory::Instance().registerElement<SVG::SVG>();
        ElementFactory::Instance().registerElement<SVG::Group>();
        ElementFactory::Instance().registerElement<SVG::Polygon>();
        ElementFactory::Instance().registerElement<SVG::Path>();
        ElementFactory::Instance().registerElement<SVG::Rect>();

        ElementFactory::Instance().registerElement<Tree>();
        ElementFactory::Instance().registerElement<TreeChildren>();
        ElementFactory::Instance().registerElement<TreeCols>();
        ElementFactory::Instance().registerElement<TreeCol>();
        ElementFactory::Instance().registerElement<TreeRow>();
        ElementFactory::Instance().registerElement<TreeCell>();
        ElementFactory::Instance().registerElement<TreeItem>();
        ElementFactory::Instance().registerElement<Statusbar>();
        ElementFactory::Instance().registerElement<StatusbarPanel>();

        ElementFactory::Instance().registerElement<Toolbar>();
        ElementFactory::Instance().registerElement<ToolbarButton>();
        
    }


    Initializer::~Initializer()
    {
        ErrorReporter::Finalize();
    }

} // namespace XULWin
