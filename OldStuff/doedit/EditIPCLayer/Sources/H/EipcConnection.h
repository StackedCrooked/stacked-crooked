#ifndef EIPCCONECTION_H
#define EIPCCONECTION_H

#include "DipcConnection.h"

#include <QTcpServer>
#include <QPointer>

class EipcConnection : public DipcConnection
{
	Q_OBJECT
public:
	static void
	Initialize();

	static void
	Finalize();

	virtual ~EipcConnection();

private slots:
	void handleNewConnection();
	void processReadyRead();

private:
	EipcConnection();
	static EipcConnection* sConnection;

	QPointer<QTcpServer> mTcpServer;
};

#endif // EIPCCONECTION_H