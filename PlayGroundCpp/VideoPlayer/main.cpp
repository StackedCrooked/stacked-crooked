#include "MainWindow.h"
#include "GStreamer.h"
#include <QtGui/QApplication>
#include <QMessageBox>
#include <string>


void ShowError(const std::string & inMessage)
{
    QMessageBox msgBox;
    msgBox.setText(inMessage.c_str());
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);
    msgBox.exec();
}


int run(QApplication & application)
{
    GStreamer::Initialize();
    MainWindow mainWindow;
    mainWindow.show();
    GStreamer::Finalize();
    return application.exec();
}


int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    try
    {
        return run(application);
    }
    catch (const std::exception & inException)
    {
        ShowError(inException.what());
    }
    return 1;
}
