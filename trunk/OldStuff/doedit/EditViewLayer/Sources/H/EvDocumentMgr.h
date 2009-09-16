#ifndef EVDOCUMENTVIEWMGR
#define EVDOCUMENTVIEWMGR

#include <QList>

#include <QPointer>
#include <QMap>

class EvDocumentView;
class QString;
class QTextDocument;

class EvDocumentMgr : public QObject
{
    Q_OBJECT
public:
	static EvDocumentMgr&
	GetInstance();

	static void
	Initialize();
	
	static void
	Finalize();

	~EvDocumentMgr();

	/// Creates a new document and retains ownership
	QTextDocument*
	New();

	/// Disposes the document without saving
	void
	ReleaseDocument( QTextDocument* inDocument );

	QTextDocument*
	Load( const QString& inFilePath );

	bool
	Save( QTextDocument* inTextDocument, QString& outErrorString );

	bool
	SaveAs( QTextDocument* inTextDocument, const QString& inFilePath, QString& outErrorString );

	bool
	Revert( QTextDocument*& );

	QString
	GetFileName( QTextDocument* ) const;

	bool
	IsNewDocument( QTextDocument* inDocument );

	bool
	IsModified( QTextDocument* inDocument ) const;

	void
	SetModified( QTextDocument* inDocument, bool inModified = false );

	QTextDocument*
	GetDocument( int inIndex ) const;

	int
	GetCount() const;

private slots:
	void receiveContentsChanged();

private:

	void
	AddDocument( const QString& inFilePath, QTextDocument* inDocument );

	QList< QTextDocument* > mDocuments;
	struct DocInfo
	{
		DocInfo() : Modified(false){}
		bool Modified;
		QString FilePath;
		QString FileName;
	};
	QMap< QTextDocument*, DocInfo > mDocInfo;
	static QPointer< EvDocumentMgr > sInstance;
};

#endif // EVDOCUMENTVIEWMGR