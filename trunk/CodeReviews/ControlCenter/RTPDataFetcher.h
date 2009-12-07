#ifndef RTPDATAFETCHER_H
#define RTPDATAFETCHER_H


#include "Poco/Mutex.h"
#include "Poco/Runnable.h"
#include "Poco/Types.h"
#include <string>
#include <list>


class RTPPacket;


/**
 * RTPDataFetcher receives RTP packets in a background thread
 * and posts them to a queue which can be accessed from the main thread.
 */
class RTPDataFetcher : public Poco::Runnable
{
public:
    RTPDataFetcher(const std::string & inHost, int inPort);

    Poco::Int64 readData(char *data, Poco::Int64 maxlen);

    virtual void run();

    void cancel();

    bool isCancelled() const;

private:
    std::string mHost;
    int mPort;
    int mBytesWritten;
    std::list<RTPPacket*> mPackets;    
    mutable Poco::Mutex mMutex;
    bool mCancel;
};


#endif // RTPDATAFETCHER_H
