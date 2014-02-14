#include <boost/circular_buffer.hpp>
#include <iostream>
#include <vector>


class SlidingWindow
{
public:
    SlidingWindow(std::size_t capacity) : mBuffer(capacity) {}

    std::size_t write(const uint8_t* data, std::size_t length)
    {
        length = std::min(length, mBuffer.reserve());
        mBuffer.insert(mBuffer.end(), data, data + length);
        return length;
    }

    std::size_t read(uint8_t* out, std::size_t length)
    {
        auto array_one = mBuffer.array_one();
        length = std::min(length, array_one.second);
        memcpy(out, array_one.first, length);
        mBuffer.erase(mBuffer.begin(), mBuffer.begin() + length);
        return length;
    }

private:
    boost::circular_buffer<uint8_t> mBuffer;
};


int main()
{
    SlidingWindow buf(100);

    std::vector<uint8_t> v10(10);
    std::vector<uint8_t> v30(30);
    std::vector<uint8_t> v50(50);

    std::size_t written = 0;

    auto n = buf.write(v50.data(), v50.size());
    written += n;
    std::cout << "n=" << n << " total=" << written << std::endl;

    n = buf.write(v30.data(), v30.size());
    written += n;
    std::cout << "n=" << n << " total=" << written << std::endl;

    n = buf.write(v30.data(), v30.size());
    written += n;
    std::cout << "n=" << n << " total=" << written << std::endl;

    n = buf.write(v10.data(), v10.size());
    written += n;
    std::cout << "n=" << n << " total=" << written << std::endl;


    // TODO: test reads


}
