#include "MainWindow.h"
#include <QtGui/QApplication>
#include <QMessageBox>
#include <string>


extern "C"
{
    #include <gst/gst.h>
    #include <glib.h>
}


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
    MainWindow mainWindow;
    mainWindow.show();
    return application.exec();
}


int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);
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
