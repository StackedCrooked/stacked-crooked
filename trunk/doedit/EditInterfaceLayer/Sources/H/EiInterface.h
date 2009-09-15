#ifndef EIINTERFACE_H
#define EIINTERFACE_H

#include <QString>

class EiInterface
{
public:
	static EiInterface&
	GetInstance();

	QString
	GetText( int inDocId );

	void
	SetText( int inDocId, const QString& inText );
};

#endif // EIINTERFACE_H