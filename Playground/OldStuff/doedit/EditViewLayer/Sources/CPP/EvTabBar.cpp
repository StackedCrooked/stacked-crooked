#include "EvTabBar.h"

#include <QTabBar>
#include <QMenu>
#include <QMouseEvent>
#include <QTextDocument>
#include <QHBoxLayout>

EvTabBar::EvTabBar()
{
	mTabBar = new QTabBar;
	mTabBar->setShape( QTabBar::RoundedNorth );
	mContextualMenu = new QMenu;
	QAction *theCloseAction = mContextualMenu->addAction( "Close" );
	connect( theCloseAction,
			 SIGNAL( triggered(bool) ),
			 this,
			 SLOT( receiveCloseAction(bool) ) );

	connect( mTabBar,
			 SIGNAL( closeCurrent() ),
			 this,
			 SLOT( CloseCurrent() ) );

	// Layouts
	QHBoxLayout *theMainLayout = new QHBoxLayout( 0 );
	theMainLayout->addWidget( mTabBar, 1 );
	this->setLayout( theMainLayout );
}

int
EvTabBar::AddTab( QTextDocument* inDocument, const QString& inTabText )
{
	mDocuments.append( inDocument );
	return mTabBar->addTab( inTabText );
}

void
EvTabBar::RemoveTab( int inIndex )
{
	mDocuments.removeAt( inIndex );
	mTabBar->removeTab( inIndex );
}

QTextDocument*
EvTabBar::GetDocument( int inTabIndex )
{
	return mDocuments[ inTabIndex ];
}

int
EvTabBar::GetIndexOf( QTextDocument* inDocument )
{
	return mDocuments.indexOf( inDocument );
}

int
EvTabBar::GetCurrentIndex()
{
	return mTabBar->currentIndex();
}

void
EvTabBar::SetCurrentIndex( int inIndex )
{
	mTabBar->setCurrentIndex( inIndex );
}

int
EvTabBar::GetCount()
{
	return mTabBar->count();
}

void
EvTabBar::CloseCurrent()
{
	if( mTabBar->count() > 1 )
	{
		int theCurrentIndex = mTabBar->currentIndex();
		mDocuments.removeAt( theCurrentIndex );
		mTabBar->removeTab( theCurrentIndex );
	}
}

void
EvTabBar::receiveCloseAction( bool )
{
	CloseCurrent();
}

void
EvTabBar::mousePressEvent ( QMouseEvent * inMouseEvent )
{
	if( inMouseEvent->button() == Qt::RightButton && mTabBar->count() > 1 )
	{
		mousePressEvent( &QMouseEvent( QEvent::MouseButtonPress, inMouseEvent->pos(), Qt::LeftButton, Qt::NoButton, Qt::NoModifier ) );
		mContextualMenu->popup( inMouseEvent->globalPos() );
	}
	QWidget::mousePressEvent( inMouseEvent );
}

