#include "Stopwatch.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread.hpp>
#include <stdexcept>


namespace Server {


unsigned GetCurrentTimeMs()
{
    boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration duration(time.time_of_day());
    return static_cast<unsigned>(duration.total_milliseconds());
}


//void Sleep(unsigned inMilliseconds)
//{
//    boost::this_thread::sleep(boost::posix_time::milliseconds(inMilliseconds));
//}


Stopwatch::Stopwatch() :
    mStart(0),
    mStop(0)
{
}


void Stopwatch::start()
{
    if (mStart == 0)
    {
        mStart = GetCurrentTimeMs();
    }
}


void Stopwatch::stop()
{
    if (mStop == 0)
    {
        mStop = GetCurrentTimeMs();
    }
}


void Stopwatch::restart()
{
    mStart = GetCurrentTimeMs();
    mStop = 0;
}


unsigned Stopwatch::elapsedMs() const
{
    if (mStart != 0)
    {
        if (mStop != 0)
        {
            return mStop - mStart;
        }
        else
        {
            return GetCurrentTimeMs() - mStart;
        }
    }
    else
    {
        return 0;
    }

}


} // namespace Server
