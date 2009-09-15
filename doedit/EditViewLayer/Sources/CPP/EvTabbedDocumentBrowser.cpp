#include "EvTabbedDocumentBrowser.h"
#include "EvDocumentMgr.h"

#include <QTabBar>
#include <QPushButton>
#include <QMenu>
#include <QMouseEvent>
#include <QVariant>
#include <QTextDocument>
#include <QVBoxLayout>
#include <QToolButton>

#include <iostream>
using std::cout;


EvTabbedDocumentBrowser::EvTabbedDocumentBrowser()
{
	// Components
	mTabBar = new EvTabBar;
	mCloseButton = new QToolButton();
	mCloseButton->setIcon( QIcon( "images/close.png" ) );
	mCloseButton->setToolButtonStyle( Qt::ToolButtonIconOnly );

	// Layouts
	QHBoxLayout *theMainLayout = new QHBoxLayout( 0 );
	theMainLayout->addWidget( mTabBar, 1 );
	theMainLayout->addWidget( mCloseButton, 0 );
	this->setLayout( theMainLayout );

	// Listeners
	connect( mTabBar, SIGNAL( currentChanged ( int ) ), this, SLOT( receiveCurrentChanged( int ) ) );
	connect( mCloseButton, SIGNAL( clicked() ), this, SLOT( receiveClicked() ) );
}
EvTabbedDocumentBrowser::~EvTabbedDocumentBrowser()
{

}

/*void
EvTabbedDocumentBrowser::mousePressEvent ( QMouseEvent * inMouseEvent )
{
}*/

void
EvTabbedDocumentBrowser::receiveClicked()
{
	// User clicked close button
	mTabBar->CloseCurrent();
}

void
EvTabbedDocumentBrowser::receiveCurrentChanged( int inIndex )
{
	emit currentChanged( mTabBar->GetCurrentIndex() );
}

void
EvTabbedDocumentBrowser::receiveCloseAction( bool )
{
	mTabBar->CloseCurrent();
}

void
EvTabbedDocumentBrowser::receiveContentsChanged ()
{
}

QTextDocument*
EvTabbedDocumentBrowser::GetSelectedDocument() const
{
	int theCurrentIndex = mTabBar->GetCurrentIndex();
	QTextDocument* theTextDocument = 0;
	if( theCurrentIndex != -1 )
	{
		theTextDocument = mTabBar->GetDocument( theCurrentIndex );
	}
	return theTextDocument;
}

void
EvTabbedDocumentBrowser::SetSelectedDocument( QTextDocument* inDocument )
{
	// Find if the document is already in the tab
	int theIndex = GetIndexOf( inDocument );
	if( theIndex != -1 )
	{
		mTabBar->SetCurrentIndex( theIndex );
	}
	else
	{
		// If not found add a new tab and corresponding data
		AddDocument( inDocument );
	}
}

int
EvTabbedDocumentBrowser::GetSelectedIndex() const
{
	return mTabBar->GetCurrentIndex();
}

void
EvTabbedDocumentBrowser::SetSelectedIndex( int inIndex )
{
	mTabBar->SetCurrentIndex( inIndex );
}

QString
EvTabbedDocumentBrowser::GetTabText( QTextDocument* inDocument )
{
	QString theFileName = EvDocumentMgr::GetInstance().GetFileName( inDocument );
	theFileName == "" ? "(untitled)" : theFileName;
	theFileName += EvDocumentMgr::GetInstance().IsNewDocument( inDocument ) ? "*" : "";
	return theFileName;
}

void
EvTabbedDocumentBrowser::AddDocument( QTextDocument* inDocument )
{
	mTabBar->blockSignals( true );
	int theTabIndex = mTabBar->AddTab( inDocument, GetTabText( inDocument ) );
	
	connect( inDocument, SIGNAL( contentsChanged() ), this, SLOT( receiveContentsChanged() ) );
	
	mTabBar->blockSignals( false );
	emit currentChanged( theTabIndex );
}

void
EvTabbedDocumentBrowser::RemoveDocument( QTextDocument* inDocument )
{
	int theIndex = GetIndexOf( inDocument );
	if( theIndex != -1 )
	{
		mTabBar->RemoveTab( theIndex );
	}
}

int
EvTabbedDocumentBrowser::GetIndexOf( QTextDocument* inDocument ) const
{
	return mTabBar->GetIndexOf( inDocument );
}