#ifndef CONTROLCENTER_H
#define CONTROLCENTER_H


#include "RTPDataFetcher.h"
#include "Poco/Thread.h"


class ControlCenter
{
public:
    ControlCenter();

    ~ControlCenter();

    RTPDataFetcher & getRTPDataFetcher() { return mRTPDataFetcher; }

private:
    Poco::Thread mThread;
    RTPDataFetcher mRTPDataFetcher;
};


#endif // CONTROLCENTER_H
