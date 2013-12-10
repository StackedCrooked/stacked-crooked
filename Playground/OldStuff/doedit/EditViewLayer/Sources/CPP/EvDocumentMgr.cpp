#include "EvDocumentMgr.h"

#include <QPointer>
#include <QTextDocument>
#include <QTextStream>
#include <QApplication>

#include <QFile>
#include <QFileInfo>

#include <iostream>
using std::cout;

QPointer< EvDocumentMgr >
EvDocumentMgr::sInstance = 0;

EvDocumentMgr&
EvDocumentMgr::GetInstance()
{
	if( !sInstance )
	{
		sInstance = new EvDocumentMgr();
	}
	return *sInstance;
}


void
EvDocumentMgr::Initialize()
{
	sInstance = new EvDocumentMgr;
}
	
void
EvDocumentMgr::Finalize()
{
	delete sInstance;
}

EvDocumentMgr::~EvDocumentMgr()
{
	for( int theIndex = 0; theIndex != mDocuments.size(); ++theIndex )
	{
		delete mDocuments[ theIndex ];
	}
}



void
EvDocumentMgr::receiveContentsChanged ()
{
	mDocInfo[ static_cast< QTextDocument* >( sender() ) ].Modified = true;
}

QTextDocument*
EvDocumentMgr::New()
{
	QTextDocument* theDocument = new QTextDocument;
	AddDocument( "", theDocument );
	return theDocument;
}

void
EvDocumentMgr::ReleaseDocument( QTextDocument* inDocument )
{
	int theIndex = mDocuments.indexOf( inDocument );
	if( theIndex >= 0 )
	{
		mDocuments.removeAt( theIndex );
		delete inDocument;
	}
}

QTextDocument*
EvDocumentMgr::Load( const QString& inFilePath )
{
	QTextDocument* theDocument = 0;
    QFile theFile( inFilePath );
    if ( theFile.open(QFile::ReadOnly | QFile::Text) )
	{
		QTextStream theTextStream( &theFile );
		QApplication::setOverrideCursor(Qt::WaitCursor);
		theDocument = new QTextDocument( theTextStream.readAll() );
		AddDocument( inFilePath, theDocument );
		QApplication::restoreOverrideCursor();
    }
	return theDocument;
}

bool
EvDocumentMgr::Save( QTextDocument* inTextDocument, QString& outErrorString )
{
	return SaveAs( inTextDocument, mDocInfo[ inTextDocument ].FilePath, outErrorString );
}

bool
EvDocumentMgr::SaveAs( QTextDocument* inTextDocument, const QString& inFilePath, QString& outErrorString )
{
	bool theResult = false;

	QFile theFile( inFilePath );
    if ( theFile.open(QFile::WriteOnly | QFile::Text) )
	{
		QTextStream theOutStream( &theFile );
		QApplication::setOverrideCursor(Qt::WaitCursor);
		theOutStream << inTextDocument->toPlainText();
		QApplication::restoreOverrideCursor();
		mDocInfo[ inTextDocument ].FileName = QFileInfo( inFilePath ).fileName();
		mDocInfo[ inTextDocument ].FilePath = inFilePath;
		mDocInfo[ inTextDocument ].Modified = false;
		theResult = true;
    }
	outErrorString = theFile.errorString();
	return theResult;
}

bool
EvDocumentMgr::Revert( QTextDocument*& inDocument )
{
	bool theResult = false;
	if( mDocuments.contains( inDocument ) )
	{
		ReleaseDocument( inDocument );
		inDocument = Load( mDocInfo[ inDocument ].FilePath );
		mDocInfo[ inDocument ].Modified = false;
		theResult = true;
	}
	return theResult;
}

bool
EvDocumentMgr::IsNewDocument( QTextDocument* inDocument )
{
	return mDocInfo[ inDocument ].FilePath == "";
}

bool
EvDocumentMgr::IsModified(QTextDocument* inDocument ) const
{
	return mDocInfo[ inDocument ].Modified;
}

void
EvDocumentMgr::SetModified( QTextDocument* inDocument, bool inModified )
{
	mDocInfo[ inDocument ].Modified = inModified;
}

QString
EvDocumentMgr::GetFileName( QTextDocument* inDocument ) const
{
	return mDocInfo[ inDocument ].FileName;
}

void
EvDocumentMgr::AddDocument( const QString& inFilePath, QTextDocument* inDocument )
{
	mDocInfo[ inDocument ].FilePath = inFilePath;
	mDocInfo[ inDocument ].Modified = false;
	mDocInfo[ inDocument ].FileName = QFileInfo( inFilePath ).fileName();
	mDocuments.append( inDocument );
	connect( inDocument, SIGNAL( contentsChanged() ), this, SLOT( receiveContentsChanged() ) );
}

QTextDocument*
EvDocumentMgr::GetDocument( int inIndex ) const
{
	if( inIndex >= 0 && inIndex < mDocuments.size() )
	{
		return mDocuments[ inIndex ];
	}
	return 0;
}

int
EvDocumentMgr::GetCount() const
{
	return mDocuments.size();
}