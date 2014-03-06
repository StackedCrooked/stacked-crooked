#include "MainWindow.h"


using namespace Mesmerize;


MainWindow::MainWindow(QWidget * inParent) :
    QMainWindow(inParent),
    mListView(new ListView),
    mListModel(new ListModel),
    mLabel(new QLabel("Label")),
    mTimer(new QTimer(this)),
    mImporter()
{

    mListView->setModel(mListModel);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setWindowTitle("Testing QListView");


    QFrame *frame = new QFrame;
    QHBoxLayout *frameLayout = new QHBoxLayout(frame);
    frameLayout->addWidget(mListView);
    frameLayout->addWidget(mLabel);

    setCentralWidget(frame);

    QString str = QFileDialog::getExistingDirectory();
    if (!str.isNull())
    {
        mImporter.reset(new Importer(str.toAscii().data()));
    }

    connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
    mTimer->start(200);
}


void MainWindow::onTimerEvent()
{
    if (!mImporter)
    {
        return;
    }
    Importer & importer = *mImporter;
    auto files = importer.stealImported();
    mListModel->addFiles(files);
    mListView->reset();
}
