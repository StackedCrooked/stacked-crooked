#include "RTPDataFetcher.h"
#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"
#include <iostream>
#include <assert.h>
#include <stdlib.h>


RTPDataFetcher::RTPDataFetcher(const std::string & inHost, int inPort) :
    mHost(inHost),
    mPort(inPort),
    mBytesWritten(0),
    mCancel(false)
{
}


Poco::Int64 RTPDataFetcher::readData(char * outBuffer, Poco::Int64 inBufferSize)
{
    Poco::ScopedLock<Poco::Mutex> lock(mMutex);
    if (mPackets.empty())
    {
        assert(mBytesWritten == 0);
        return 0;
    }

    RTPPacket * packet = *mPackets.begin();
    uint8_t * data = packet->GetPacketData() + mBytesWritten;
    size_t packetLength = packet->GetPacketLength() - mBytesWritten;

    Poco::Int64 bytesWritten = 0;
    Poco::Int64 bytesRead = 0;
    while (bytesWritten < inBufferSize && bytesRead < packetLength)
    {
        *outBuffer = *data;
        ++outBuffer;
        ++data;
        ++bytesWritten;
        ++bytesRead;
    }

    if (bytesRead == packetLength)
    {
        delete packet;
        mPackets.erase(mPackets.begin());
        mBytesWritten = 0;
    }
    else
    {
        mBytesWritten += bytesWritten;
    }

    return bytesWritten;
}


void RTPDataFetcher::cancel()
{
    Poco::ScopedLock<Poco::Mutex> lock(mMutex);
    mCancel = true;
}


bool RTPDataFetcher::isCancelled() const
{
    Poco::ScopedLock<Poco::Mutex> lock(mMutex);
    return mCancel;
}


void RTPDataFetcher::run()
{
    RTPSession session;

    RTPSessionParams sessionparams;
    sessionparams.SetOwnTimestampUnit(1.0/8000.0);

    RTPUDPv4TransmissionParams transparams;
    transparams.SetPortbase(mPort);

    int status = session.Create(sessionparams,&transparams);
    if (status < 0)
    {
        std::cerr << RTPGetErrorString(status) << std::endl;
        exit(-1);
    }


    session.SetDefaultPayloadType(96);
    session.SetDefaultMark(false);
    session.SetDefaultTimestampIncrement(160);

    uint8_t silencebuffer[160];
    for (int i = 0 ; i < 160 ; i++)
        silencebuffer[i] = 128;

    RTPTime delay(0.020);
    RTPTime starttime = RTPTime::CurrentTime();
    
    bool done = false;
    while (!done)
    {
        if (isCancelled())
        {
            break;
        }

        status = session.SendPacket(silencebuffer,160);
        if (status < 0)
        {
            std::cerr << RTPGetErrorString(status) << std::endl;
            exit(-1);
        }

        session.BeginDataAccess();
        if (session.GotoFirstSource())
        {
            do
            {
                RTPPacket *packet;

                while ((packet = session.GetNextPacket()) != 0)
                {
                    Poco::ScopedLock<Poco::Mutex> lock(mMutex);
                    mPackets.push_back(packet);
                }

            } while (session.GotoNextSource());
        }
        session.EndDataAccess();

        RTPTime::Wait(delay);

        RTPTime t = RTPTime::CurrentTime();
        t -= starttime;
        if (t > RTPTime(60.0))
            done = true;
    }

    delay = RTPTime(10.0);
    session.BYEDestroy(delay,"Time's up",9);
}

