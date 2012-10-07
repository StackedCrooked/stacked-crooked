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


// A cache_aligned_allocator allocates memory on cache line boundaries, in order
// to avoid false sharing. False sharing is when logically distinct items occupy
// the same cache line, which can hurt performance if multiple threads attempt
// to access the different items simultaneously. Even though the items are
// logically separate, the processor hardware may have to transfer the cache
// line between the processors as if they were sharing a location. The net result
// can be much more memory traffic than if the logically distinct items were on
// different cache lines.
typedef std::vector<uint64_t, tbb::cache_aligned_allocator<int> > CacheAlignedContainer;
typedef std::vector<uint64_t> StandardContainer;


std::mutex m;


template<typename Container>
void test_impl()
{
    Stopwatch sw;
    Container a(10 * 1000 * 1000);
    std::generate(a.begin(), a.end(), [&](){ static uint64_t size = 0; return size++; });
    std::cout << a[0] << ", " << a[1] << ", " << a[2] << std::endl;
    Container b = a;
    Container c = b;
    std::size_t index = 0;
    for (auto & a_el : a)
    {
        a_el = b[index] + c[index];
        index++;
    }

    std::unique_lock<std::mutex> lock(m);
    std::cout << typeid(Container).name() << ": " << sw.elapsed() << " s." << std::endl;
}


void test()
{
    std::vector<std::thread> vec;
    vec.emplace_back(std::bind(&test_impl<CacheAlignedContainer>));
    test_impl<StandardContainer>();


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
