#include "CentralWidget.h"


CentralWidget::CentralWidget(QWidget *inParent) :
    QWidget(inParent),
    mListModel(new ListModel()),
    mListItemDelegate(new ListItemDelegate(this)),
    mListView(new ListView(this, mListModel, mListItemDelegate)),
    mImageWidget(new ImageWidget(this))
{
    QHBoxLayout * hbox = new QHBoxLayout();
    hbox->addWidget(mListView, 0);
    hbox->addWidget(mImageWidget, 1);
    this->setLayout(hbox);
}


void CentralWidget::setFile(const QString & inFile)
{
    mImageWidget->setFile(inFile);
}


QSize CentralWidget::sizeHint() const
{
    return mListView->sizeHint() + mImageWidget->sizeHint();
}

