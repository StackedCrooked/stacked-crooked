#include "EiInterface.h"

#include "EvDocumentMgr.h"
#include <QTextEdit>
#include <QTextDocument>

EiInterface&
EiInterface::GetInstance()
{
	static EiInterface fInstance;
	return fInstance;
}

QString
EiInterface::GetText( int inDocId )
{
	return EvDocumentMgr::GetInstance().GetDocument( inDocId )->toPlainText();
}

void
EiInterface::SetText( int inDocId, const QString& inText )
{
	return EvDocumentMgr::GetInstance().GetDocument( inDocId )->setPlainText( inText );
}