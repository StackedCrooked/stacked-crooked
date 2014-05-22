#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include "miniz.c"

#if 0

Memory to Memory Compression
============================

  // Returns Z_OK on success, or one of the error codes from deflate() on failure.
  int compress(Byte *pDest, uLong *pDest_len, const Byte *pSource, uLong source_len);

  // Like compress() but with more control, level may range from 0 (storing) to 9 (max. compression)
  int compress2(Byte *pDest, uLong *pDest_len, const Byte *pSource, uLong source_len, int level);


Memory to Memory Decompression
==============================

  // Returns Z_OK on success, or one of the error codes from inflate() on failure.
  int uncompress(Byte *pDest, uLong *pDest_len, const Byte *pSource, uLong source_len);

#endif



using Blob = std::vector<unsigned char>;


#include <chrono>

struct Stopwatch
{
    typedef std::chrono::high_resolution_clock Clock;

    Stopwatch() : mStartTime(Clock::now()) {}

    long elapsed_us() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - mStartTime).count();
    }

private:
    Clock::time_point mStartTime;
};



Blob zip(const Blob& blob, int level)
{
    Blob result(2 * blob.size());
    std::size_t length = result.size();
    if (int error = compress2(result.data(), &length, blob.data(), blob.size(), level))
    {
        throw std::runtime_error("Compression failed. Error code: " + std::to_string(error) + " length=" + std::to_string(length));
    }
    result.resize(length);
    return result;
}


Blob unzip(const Blob& blob)
{
    Blob result(blob.size() * 10);

    // NOTE: Very naive algorithm: grow output buffer until uncompress succceeds.
    for (;;)
    {
        auto length = result.size();
        auto err = uncompress(result.data(), &length, blob.data(), blob.size());

        if (err == Z_OK)
        {
            result.resize(length);
            return result;
        }
        result.resize(result.size() * 2);
    }
}


std::string read_file(const std::string& file)
{
    std::string result;
    std::ifstream t(file);
    t.seekg(0, std::ios::end);
    result.reserve(t.tellg());
    t.seekg(0, std::ios::beg);
    result.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return result;
}



void test(const std::string& s, int level)
{
    Blob orig_blob(s.size());
    memcpy(orig_blob.data(), s.data(), s.size());

    Stopwatch sw;
    auto zipped_blob = zip(orig_blob, level);
    auto zip_time = sw.elapsed_us();

    sw = Stopwatch();
    auto unzipped_blob = unzip(zipped_blob);
    auto unzip_time = sw.elapsed_us();


    assert(orig_blob == unzipped_blob);


    std::cout << "level=" << level << " orig_size=" << s.size() << " compressed_size=" << zipped_blob.size() << " zip_time=" << zip_time << "us unzip_time=" << unzip_time << "us ratio=" << int(0.5 + 100.0 * zipped_blob.size() / unzipped_blob.size()) << std::endl;
}


int main()
{
    uint64_t sum_chars = 0;
    auto s = read_file("main.cpp");
    s.resize(std::min(s.size(), 100000ul));
    for (auto&& c : s)
    {
        sum_chars += c;
    }
    std::cout << sum_chars << std::endl;

    test(s, MZ_DEFAULT_COMPRESSION);
}
