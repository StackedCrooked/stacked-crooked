#ifndef SERVER_STOPWATCH_H
#define SERVER_STOPWATCH_H


#include <boost/noncopyable.hpp>
#include <ctime>


class Stopwatch : boost::noncopyable
{
public:
    /// Creates and starts the stopwatch.
    Stopwatch();

    void start();

    void stop();

    /// Reset and start the stopwatch.
    void restart();

    /// Returns the elapsed time in milliseconds.
    unsigned elapsedMs() const;

private:
    unsigned mStart;
    unsigned mStop;
};


#endif // SERVER_STOPWATCH_H
