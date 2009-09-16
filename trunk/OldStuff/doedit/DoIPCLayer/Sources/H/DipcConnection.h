#ifndef DIPCCONECTION_H
#define DIPCCONECTION_H

#include <QObject>

class DipcConnection : public QObject
{
	Q_OBJECT
public:
	virtual ~DipcConnection() = 0;
private:
};

#endif DIPCCONECTION_H
