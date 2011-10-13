#ifndef GST_LOGGER_H
#define GST_LOGGER_H


#include "Gst/MakeString.h"
#include "Gst/Threading.h"
#include <string>
#include <vector>
#include <boost/function.hpp>


namespace Gst {


enum LogLevel
{
    LogLevel_Debug,
    LogLevel_Info,
    LogLevel_Warning,
    LogLevel_Error
};

std::string ToString(LogLevel inLogLevel);

class Logger
{
public:
    static Logger & Instance();

    typedef boost::function<void(const std::string &)> LogHandler;

    void setLogHandler(const LogHandler & inHandler);

    void log(LogLevel inLogLevel, const std::string & inMessage);

    // Messages posted from worker threads are stored in a queue.
    // The actual logging is delayed until:
    //   - a log message is posted from the main thread
    //   - flush() is called
    //
    // This method should probably only be called from the main thread.
    void flush();

private:
    void logImpl(const std::string & inMessage);

    LogHandler mHandler;
    typedef std::vector<std::string> Queue;
    Protected<Queue> mProtectedQueue;
    boost::mutex mQueueMutex;
};


void LogDebug(const std::string & inMessage);
void LogInfo(const std::string & inMessage);
void LogWarning(const std::string & inMessage);
void LogError(const std::string & inMessage);



class Info
{
public:
   template <typename T>
   Gst::Info& operator<<(const T & datum)
   {
      mBuffer << datum;
      return *this;
   }
   
   ~Info()
   {
       LogInfo(mBuffer.str());
   }
private:
   std::ostringstream mBuffer;
};



} // namespace Gst


#endif // GST_LOGGER_H
