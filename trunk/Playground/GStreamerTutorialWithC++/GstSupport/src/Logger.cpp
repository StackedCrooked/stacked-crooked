#include "Gst/Logger.h"
#include <stdexcept>


namespace Gst {


void LogDebug(const std::string & inMessage)
{
    Logger::Instance().log(LogLevel_Debug, inMessage);
}


void LogInfo(const std::string & inMessage)
{
    Logger::Instance().log(LogLevel_Info, inMessage);
}


void LogWarning(const std::string & inMessage)
{
    Logger::Instance().log(LogLevel_Warning, inMessage);
}


void LogError(const std::string & inMessage)
{
    Logger::Instance().log(LogLevel_Error, inMessage);
}


std::string ToString(LogLevel inLogLevel)
{
    switch (inLogLevel)
    {
        case LogLevel_Debug:
        {
            return "DEBUG";
        }
        case LogLevel_Info:
        {
            return "INFO";
        }
        case LogLevel_Warning:
        {
            return "WARNING";
        }
        case LogLevel_Error:
        {
            return "ERROR";
        }
        default:
        {
            throw std::invalid_argument("Invalid LogLevel enum value");
        }
    }
}


std::string GetMessage(LogLevel inLogLevel, const std::string & inMessage)
{
    return ToString(inLogLevel) + ": " + inMessage;
}


Logger & Logger::Instance()
{
    static Logger fInstance;
    return fInstance;
}


void Logger::setLogHandler(const LogHandler & inHandler)
{
    mHandler = inHandler;
}


void Logger::flush()
{
    std::vector<std::string> items;
    items.reserve(100);


    // Since the logging can be a slow operation we don't keep the Game object locked here.
    // We just copy the items, and log them afterwards.
    {
        ScopedReaderAndWriter<Queue> queue(mProtectedQueue);
        for (size_t idx = 0; idx != queue->size(); ++idx)
        {
            items.push_back((*queue.get())[idx]);
        }   
        queue->clear();
    }
    

    // Ok, the game object is unlocked again. We can log the items here.
    for (size_t idx = 0; idx != items.size(); ++idx)
    {
        logImpl(items[idx]);
    }
}


void Logger::logImpl(const std::string & inMessage)
{
    if (mHandler)
    {
        mHandler(inMessage);
    }
}


void Logger::log(LogLevel inLogLevel, const std::string & inMessage)
{
    // Critical section: add the log message to the buffer
    {
        ScopedReaderAndWriter<Queue> queue(mProtectedQueue);
        queue->push_back(GetMessage(inLogLevel, inMessage));
    }
}


} // namespace Gst
