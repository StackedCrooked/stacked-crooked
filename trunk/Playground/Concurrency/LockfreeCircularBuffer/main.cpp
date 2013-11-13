#include <boost/circular_buffer.hpp>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>
#include <stdint.h>
#include "tbb/concurrent_queue.h"


using namespace std::chrono;
typedef high_resolution_clock Clock;


std::vector<uint8_t> generateTestData(unsigned n);
void print(std::string str);
inline std::ostream& operator<<(std::ostream& os, const std::vector<uint8_t>& vec);


struct MakeString
{
    template <typename T>
    MakeString & operator<<(const T & datum);
    MakeString & operator<<(uint8_t n);
    operator std::string() const;
    std::string str() const;
    std::ostringstream mBuffer;
};


typedef std::vector<uint8_t> Segment;

struct ConcurrentBuffer
{
    ConcurrentBuffer()
    {
    }

    Segment* read()
    {
        Segment* segment;
        while (!segments.try_pop(segment))
        {
            std::this_thread::yield();
        }
        return segment;
    }

    void write(Segment* segment)
    {
        segments.push(std::move(segment));
    }


    tbb::concurrent_bounded_queue<Segment*> segments;
};


int main()
{
    ConcurrentBuffer buf;

    // The writer will write this to the buffer until all data has been sent.
    // The reader will read from this buffer until all test data has been received.

    const auto total_size = 10 * 1000 * 1000 * 1536UL;
    tbb::concurrent_bounded_queue<Segment*> pool;;
    for (int i = 0; i != 300; ++i)
    {
        pool.push(new Segment(1536));
        //std::cout << "Pushed segment to pool" << std::endl;
    }


    //
    // Reader thread
    //
    std::thread t([&] {
        auto total_read = 0UL;
        while (total_read != total_size) {
            Segment* segment = buf.read();
            total_read += segment->size();
            pool.push(segment);
            //std::cout << "Pushed segment back to pool" << std::endl;
        }
    });


	auto start = Clock::now();


    //
    // Write thread (main thread)
    //
    std::atomic<uint64_t> total_written{0};
    while (total_written != total_size)
    {
        Segment* segment;
        while (!pool.try_pop(segment))
        {
            std::this_thread::yield();
        }
        auto n = segment->size();
        //std::cout << "Popped segment from pool." << std::endl;
        buf.write(segment);
        total_written += n;
    }

	t.join();

	auto us = duration_cast<microseconds>(Clock::now() - start).count();
	std::cout << static_cast<int>(8.0 * total_written / (1000.0 * us)) << "Gbps" << std::endl;
}
