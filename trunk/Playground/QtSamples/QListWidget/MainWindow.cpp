#include "MainWindow.h"
#include "Poco/Path.h"


using namespace Mesmerize;


MainWindow::MainWindow(QWidget * inParent) :
    QMainWindow(inParent),
    mListWidget(),
    mImporter()
{
    mListWidget = new QListWidget(this);

    mListWidget->setIconSize(QSize(200, 200));
    mListWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    mListWidget->setMaximumWidth(240);
    mListWidget->setSpacing(10);
    mListWidget->setMovement(QListView::Static);
    setCentralWidget(mListWidget);

    import("/Volumes/WesternDigital/Media/Anime");


    QTimer * timer  = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
    timer->start(200);
}


void MainWindow::import(const std::string & inPath)
{
    mImporter.reset(new Importer(inPath));
}


void MainWindow::onTimerEvent()
{
    for (auto & file : mImporter->stealImported())
    {
        QListWidgetItem * item = new QListWidgetItem(mListWidget);
        item->setIcon(QIcon(QString(file.c_str())));
        //item->setText(Poco::Path(file).getBaseName().c_str());
        item->setTextAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    }
}
