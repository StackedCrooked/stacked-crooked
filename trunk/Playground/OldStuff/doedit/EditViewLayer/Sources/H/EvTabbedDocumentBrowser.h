#ifndef EVTABBEDDOCUMENTBROWSER
#define EVTABBEDDOCUMENTBROWSER

#include <QMap>
#include <QMenu>
#include <QTabBar>

#include "EvDocumentBrowser.h"
#include "EvTabBar.h"

class QTextDocument;
class QPushButton;


class EvTabbedDocumentBrowser : public EvDocumentBrowser
{
    Q_OBJECT
public:
	EvTabbedDocumentBrowser();
	virtual ~EvTabbedDocumentBrowser();

	virtual QTextDocument*
	GetSelectedDocument() const;

	virtual void
	SetSelectedDocument( QTextDocument* inDocument );

	int
	GetSelectedIndex() const;

	void
	SetSelectedIndex( int inIndex );

	QString
	GetTabText( QTextDocument* inDocument );

	void
	AddDocument( QTextDocument* inDocument );

	void
	RemoveDocument( QTextDocument* inDocument );

	int
	GetIndexOf( QTextDocument* inDocument ) const;

signals:
	void
	currentChanged ( int inIndex );

protected:
	/*void
	mousePressEvent ( QMouseEvent * inMouseEvent );*/

private slots:
	void
	receiveCurrentChanged( int inIndex );

	void
	receiveContentsChanged();

	void
	receiveCloseAction( bool );

	void
	receiveClicked();

private:

	EvTabbedDocumentBrowser( const EvTabbedDocumentBrowser& );
	EvTabbedDocumentBrowser& operator= ( const EvTabbedDocumentBrowser& );

	
	EvTabBar* mTabBar;
	QToolButton* mCloseButton;
};

#endif // EVTABBEDDOCUMENTBROWSER