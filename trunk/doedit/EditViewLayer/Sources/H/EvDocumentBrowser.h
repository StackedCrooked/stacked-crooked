#ifndef EVDOCUMENTBROWSER
#define EVDOCUMENTBROWSER

#include <QWidget>

class QTextDocument;

class EvDocumentBrowser : public QWidget
{
    Q_OBJECT
public:
	virtual ~EvDocumentBrowser() = 0 {}

	virtual QTextDocument*
	GetSelectedDocument() const = 0;

	virtual void
	SetSelectedDocument( QTextDocument* inDocument ) = 0;
};

#endif // EVDOCUMENTBROWSER