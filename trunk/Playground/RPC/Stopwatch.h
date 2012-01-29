#ifndef SERVER_STOPWATCH_H
#define SERVER_STOPWATCH_H


#include <boost/noncopyable.hpp>
#include <ctime>
#include <string>


class Stopwatch : boost::noncopyable
{
public:
    /// Creates and starts the stopwatch.
    Stopwatch(const std::string & inName);

    const std::string & name() const { return mName; }

    void start();

    unsigned stop();

    /// Reset and start the stopwatch.
    void restart();

    /// Returns the elapsed time in milliseconds.
    unsigned elapsedMs() const;

private:
    std::string mName;
    unsigned mStart;
    unsigned mStop;
};


#endif // SERVER_STOPWATCH_H
