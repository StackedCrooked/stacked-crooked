#include "Test/Tester.h"
#include "XULWin/Element.h"
#include "XULWin/XULRunner.h"
#include "Utils/WinUtils.h"


using namespace Utils;
using namespace XULWin;


namespace XULWin
{

    void Tester::runXULSample(const std::string & inAppName)
    {
        std::string chdir = "../xulrunnersamples/" + inAppName + "/";
        CurrentDirectoryChanger curdir(chdir);

        //system("run.bat");

        XULRunner runner;
        runner.run("application.ini");
    }


    void Tester::runNonXULSample()
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


} // namespace XULWin
