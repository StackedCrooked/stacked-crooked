#include "ControlCenter.h"


ControlCenter::ControlCenter() :
    mRTPDataFetcher("localhost", 4444)
{
    mThread.start(mRTPDataFetcher);
}


ControlCenter::~ControlCenter()
{
    mRTPDataFetcher.cancel();
    mThread.join();
}
