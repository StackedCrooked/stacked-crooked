#include "PCAPWriter.h"
#include "tbb/concurrent_queue.h"
#include <cstdint>
#include <fstream>
#include <thread>
#include <sys/time.h>
#include "pcap.h"


struct PCAPWriter::Impl
{
    Impl(const std::string& file) :
        mOutput(file, std::ios::out | std::ios::binary),
        mThread([this]{ consumer_thread(); })
    {
    }

    ~Impl()
    {
        mQueue.push(""); // stop-flag for the consumer thread
        mThread.join();
    }

    void push_back(const uint8_t* bytes, uint32_t len)
    {
        struct PCAPHeader
        {
            uint32_t sec;
            uint32_t usec;
            uint32_t cap_len;
            uint32_t len;
        };

        timeval tv = to_timeval(std::chrono::system_clock::now());

        PCAPHeader header{};
        header.sec = tv.tv_sec;
        header.usec = tv.tv_usec;
        header.cap_len = len;
        header.len = len;

        std::string str;
        str.reserve(sizeof(header) + len);
        str.append(cstr(&header), sizeof(header));
        str.append(cstr(bytes), len);
        mQueue.push(str);
    }

    void consumer_thread()
    {
        write_pcap_file_header();

        for (;;)
        {
            std::string pcap_string;
            mQueue.pop(pcap_string);

            if (pcap_string.empty())
            {
                // empty string is stop-flag
                return;
            }

            mOutput.write(pcap_string.data(), pcap_string.size());
        }
    }

    // Writes the initial pcap file header.
    void write_pcap_file_header()
    {
        pcap_file_header header;
        header.magic = 0xa1b2c3d4;
        header.version_major = PCAP_VERSION_MAJOR;
        header.version_minor = PCAP_VERSION_MINOR;
        header.thiszone = 0;
        header.sigfigs = 0;
        header.snaplen = 65535;
        header.linktype = 1;

        mOutput.write(cstr(&header), sizeof(header));
    }

    static const char* cstr(const void* bytes)
    {
        return reinterpret_cast<const char*>(bytes);
    }

    static timeval to_timeval(std::chrono::system_clock::time_point tp)
    {
        using namespace std::chrono;
        auto s = time_point_cast<seconds>(tp);
        if (s > tp)
            s = s - seconds{1};
        auto us = duration_cast<microseconds>(tp - s);
        timeval tv;
        tv.tv_sec = s.time_since_epoch().count();
        tv.tv_usec = us.count();
        return tv;
    }

    std::ofstream mOutput;
    tbb::concurrent_bounded_queue<std::string> mQueue;
    std::thread mThread;
};


PCAPWriter::PCAPWriter(const std::string& file) :
    mImpl(new Impl(file))
{
}


PCAPWriter::~PCAPWriter()
{
}


void PCAPWriter::push_back(const uint8_t* bytes, uint32_t len)
{
    mImpl->push_back(bytes, len);
}

