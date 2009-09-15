/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>
#include <iostream>
using std::cout;

#include "EvMainWindow.h"
#include "EvDocumentMgr.h"


EvMainWindow* EvMainWindow::sInstance = 0;

void
EvMainWindow::Finalize()
{
	delete sInstance;
	sInstance = 0;
}

EvMainWindow&
EvMainWindow::GetInstance()
{
	if( sInstance == 0 )
	{
		sInstance = new EvMainWindow();
	}
	return *sInstance;
}

EvMainWindow::EvMainWindow()
{
	// fr: need to add tabbar + docview here + layout
	mTabbedDocumentBrowser = new EvTabbedDocumentBrowser();
	mTextEdit = new QTextEdit( this );

	QWidget *theCentralWidget = new QWidget;
    QVBoxLayout *theMainLayout = new QVBoxLayout;
	theMainLayout->addWidget( mTabbedDocumentBrowser, 0, Qt::AlignTop );
	theMainLayout->addWidget( mTextEdit, 1 );

	theCentralWidget->setLayout( theMainLayout );
	setCentralWidget( theCentralWidget );
	
    createActions();
    createMenus();
    createToolBars();

    readSettings();

	// Listeners
	connect( mTabbedDocumentBrowser, SIGNAL( currentChanged( int ) ), this, SLOT( receiveCurrentChanged( int ) ) );

}

void
EvMainWindow::receiveCurrentChanged( int inIndex )
{
	mTextEdit->setDocument( mTabbedDocumentBrowser->GetSelectedDocument() );
}

void EvMainWindow::closeEvent(QCloseEvent *event)
{
	writeSettings();
}

void EvMainWindow::NewFile()
{
	QTextDocument* theDocument = EvDocumentMgr::GetInstance().New();
	mTabbedDocumentBrowser->AddDocument( theDocument );
}

void EvMainWindow::receiveOpen()
{
	QString theFileName = QFileDialog::getOpenFileName(this);
    if (!theFileName.isEmpty())
	{
		QTextDocument* theTextDocument = EvDocumentMgr::GetInstance().Load( theFileName );
		mTabbedDocumentBrowser->AddDocument( theTextDocument );
    }
}

bool EvMainWindow::receiveSave()
{
	bool theResult = false;
	QTextDocument* theTextDocument = mTabbedDocumentBrowser->GetSelectedDocument();
	QString theErrorString;
	if (EvDocumentMgr::GetInstance().IsNewDocument( theTextDocument ) )
	{
		theResult = receiveSaveAs();
	}
	else
	{
		theResult = EvDocumentMgr::GetInstance().Save( theTextDocument, theErrorString );
		if( !theResult)
		{	
			QMessageBox::warning(
				this,
				tr("Application"),
				tr("Cannot write file %1:\n%2.")
				.arg( EvDocumentMgr::GetInstance().GetFileName( theTextDocument ) )
				.arg( theErrorString ) );
		}
	}
	return theResult;
}

bool EvMainWindow::receiveSaveAs()
{
	bool theResult = false;
	QTextDocument* theTextDocument = mTabbedDocumentBrowser->GetSelectedDocument();
    QString theFileName = QFileDialog::getSaveFileName(this);
    if (!theFileName.isEmpty())
	{
		EvDocumentMgr::GetInstance().SaveAs( theTextDocument, theFileName, theFileName );
		theResult = true;
	}
	return theResult;
}

bool
EvMainWindow::receiveRevertFile()
{
	bool theResult = false;
	QTextDocument* theTextDocument = mTabbedDocumentBrowser->GetSelectedDocument();
    QString theFileName = QFileDialog::getSaveFileName(this);
    if (!theFileName.isEmpty())
	{
		theResult = EvDocumentMgr::GetInstance().Revert( theTextDocument );
	}
	return theResult;
}

void EvMainWindow::receiveAbout()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

void EvMainWindow::createActions()
{
	newAct = new QAction(QIcon("images/new.png"), tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(NewFile()));

    openAct = new QAction(QIcon("images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(receiveOpen()));

    saveAct = new QAction(QIcon("images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(receiveSave()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(receiveSaveAs()));

    revertFileAct = new QAction(tr("Revert"), this);
    revertFileAct->setStatusTip(tr("Revert the file"));
    connect(revertFileAct, SIGNAL(triggered()), this, SLOT(receiveRevertFile()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    cutAct = new QAction(QIcon("images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
	connect(cutAct, SIGNAL(triggered()), mTextEdit, SLOT(cut()));

    copyAct = new QAction(QIcon("images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), mTextEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon("images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), mTextEdit, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(mTextEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(mTextEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
}

void EvMainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void EvMainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

void EvMainWindow::readSettings()
{
    QSettings settings("Trolltech", "Application Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void EvMainWindow::writeSettings()
{
    QSettings settings("Trolltech", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}
