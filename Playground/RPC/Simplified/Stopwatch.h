#ifndef STOPWATCH_H
#define STOPWATCH_H


#include <boost/noncopyable.hpp>
#include <ctime>
#include <string>


unsigned GetCurrentTimeMs();


class Stopwatch : boost::noncopyable
{
public:
    /// Creates and starts the stopwatch.
    Stopwatch(const std::string & inName);

    long id() const { return mId; }

    const std::string & name() const { return mName; }

    void start();

    unsigned stop();

    /// Reset and start the stopwatch.
    void restart();

    /// Returns the elapsed time in milliseconds.
    unsigned elapsedMs() const;

private:
    long mId;
    std::string mName;
    unsigned mStart;
    unsigned mStop;
};


#endif // STOPWATCH_H
