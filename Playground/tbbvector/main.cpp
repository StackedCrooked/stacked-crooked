#include <QApplication>
#include "MainWindow.h"
#include "tbb/cache_aligned_allocator.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <typeinfo>


namespace {


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

typedef std::vector<uint64_t, tbb::cache_aligned_allocator<int> > CacheAlignedContainer;
typedef std::vector<uint64_t> StandardContainer;


template<typename Container>
double test_impl()
{
    Stopwatch sw;
    Container a(10 * 1000 * 1000);
    std::size_t size = 0;
    std::generate(a.begin(), a.end(), [&](){ return size++; });
    std::cout << a[0] << ", " << a[1] << ", " << a[2] << std::endl;
    Container b = a;
    Container c = b;
    std::size_t index = 0;
    for (auto & a_el : a)
    {
        a_el = b[index] + c[index];
        index++;
    }
    return sw.elapsed();
}


void test()
{
    std::cout << "CacheAlignedContainer: " << test_impl<CacheAlignedContainer>() << std::endl;
    std::cout << "StandardContainer: "     << test_impl<StandardContainer>()     << std::endl;
}


} // anonymous namespace


int main(int argc, char *argv[])
{
    test();
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
}
