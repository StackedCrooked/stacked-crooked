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

    std::size_t readable() const
    {
        return mBuffer.size();
    }

    std::size_t writable() const
    {
        return mBuffer.reserve();
    }

private:
    boost::circular_buffer<uint8_t> mBuffer;
};


int main()
{
    SlidingWindow buf(80);

    std::vector<uint8_t> v10(10);
    std::vector<uint8_t> v30(30);
    std::vector<uint8_t> v50(50);

    auto w = buf.write(v30.data(), v30.size());
    std::cout << "+" << w;
    std::cout << " readable=" << buf.readable() << " writable=" << buf.writable() << std::endl;

    w = buf.write(v30.data(), v30.size());
    std::cout << "+" << w;
    std::cout << " readable=" << buf.readable() << " writable=" << buf.writable() << std::endl;

    w = buf.write(v30.data(), v30.size());
    std::cout << "+" << w;
    std::cout << " readable=" << buf.readable() << " writable=" << buf.writable() << std::endl;

    w = buf.write(v10.data(), v10.size());
    std::cout << "+" << w;
    std::cout << " readable=" << buf.readable() << " writable=" << buf.writable() << std::endl;


    auto r = buf.read(v10.data(), v10.size());
    std::cout << "-" << r;
    std::cout << " readable=" << buf.readable() << " writable=" << buf.writable() << std::endl;

    r = buf.read(v30.data(), v30.size());
    std::cout << "-" << r;
    std::cout << " readable=" << buf.readable() << " writable=" << buf.writable() << std::endl;

    r = buf.read(v50.data(), v50.size());
    std::cout << "-" << r;
    std::cout << " readable=" << buf.readable() << " writable=" << buf.writable() << std::endl;

    w = buf.write(v10.data(), v10.size());
    std::cout << "+" << w;
    std::cout << " readable=" << buf.readable() << " writable=" << buf.writable() << std::endl;

    r = buf.read(v50.data(), v50.size());
    std::cout << "-" << r;
    std::cout << " readable=" << buf.readable() << " writable=" << buf.writable() << std::endl;
}
