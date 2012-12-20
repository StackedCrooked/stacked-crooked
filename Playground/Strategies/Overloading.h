#ifndef Overloading_H
#define Overloading_H


#include <chrono>
#include <deque>
#include <list>
#include <set>
#include <vector>


namespace Overloading {


class Stopwatch
{
public:
    typedef std::chrono::high_resolution_clock Clock;

    //! Constructor starts the stopwatch
    Stopwatch() : mStart(Clock::now())
    {
    }

    //! Returns elapsed number of seconds in decimal form.
    double elapsed()
    {
        return 1.0 * (Clock::now() - mStart).count() / Clock::period::den;
    }

    Clock::time_point mStart;
};


template<typename T> void append(std::vector<T> & vec, const T & t) { vec.push_back(t); }
template<typename T> void append(std::set<T>    & set, const T & t) { set.insert(t);    }
template<typename T> void append(std::deque<T>  & deq, const T & t) { deq.push_back(t); }
template<typename T> void append(std::list<T>   & lst, const T & t) { lst.push_back(t); }


template<typename Container>
double benchmark_container_growth()
{
    Container c;
    Stopwatch sw;
    for (int i = 0; i != 1000000; ++i)
    {
        append(c, i);
    }
    return sw.elapsed();
}


void test()
{
    benchmark_container_growth< std::vector<int> >();
    benchmark_container_growth< std::set<int>    >();
    benchmark_container_growth< std::list<int>   >();
    benchmark_container_growth< std::deque<int>  >();
}



} // namespace Overloading


#endif // Overloading_H
