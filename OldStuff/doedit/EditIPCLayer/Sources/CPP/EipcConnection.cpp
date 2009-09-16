#include "EipcConnection.h"
#include "EiInterface.h"

#include <QtGui>
#include <QtNetwork>
#include <QHostAddress>

EipcConnection* EipcConnection::sConnection = 0;

void
EipcConnection::Initialize()
{
	if( sConnection == 0 )
	{
		sConnection = new EipcConnection;
	}
}

void
EipcConnection::Finalize()
{
	delete sConnection;
	sConnection = 0;
}

EipcConnection::EipcConnection()
: mTcpServer( new QTcpServer( this ) )
{
	mTcpServer->listen( QHostAddress("Localhost"), 8080 );
    connect(mTcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
}

EipcConnection::~EipcConnection()
{
}

void EipcConnection::handleNewConnection()
{
	QTcpSocket* theClientConnection = mTcpServer->nextPendingConnection();
	connect(theClientConnection, SIGNAL(disconnected()), this, SLOT(deleteLater()));
	connect(theClientConnection, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
}

void EipcConnection::processReadyRead()
{
	QAbstractSocket* theClient = static_cast< QTcpSocket* >( QObject::sender() );
	QByteArray theData = theClient->readAll();
	EiInterface::GetInstance().SetText( 0, theData );
}