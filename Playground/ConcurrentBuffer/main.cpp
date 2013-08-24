#include <boost/circular_buffer.hpp>
#include <atomic>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>
#include <stdint.h>


struct ConcurrentBuffer
{
    ConcurrentBuffer(std::size_t inSize) :
        mBuffer(inSize),
        mNumRead(0),
        mNumWritten(0)
    {
    }

    std::size_t size() const
    {
        return mBuffer.size();
    }

    bool empty() const
    {
        return mBuffer.empty();
    }

    std::size_t read(uint8_t* buf, std::size_t len)
    {
        std::size_t numRead = mNumRead;
        std::size_t numWritten = mNumWritten;

        assert(numWritten >= numRead);

        auto numUnread = std::min(len, numWritten - numRead);
        if (numUnread == 0)
        {
            return 0;
        }

        auto begin = mBuffer.begin() + mNumRead;
        mBuffer.resize(mBuffer.size() + numUnread);

        memcpy(buf, &*begin, numUnread);
        mNumRead += numUnread;
        return numUnread;
    }

    std::size_t write(uint8_t* buf, std::size_t len)
    {
        std::size_t numRead = mNumRead;
        std::size_t numWritten = mNumWritten;

        assert(numWritten <= numRead);
        auto availableSpace = std::min(len, numWritten - numRead);


        auto begin = mBuffer.begin() + mNumWritten;
        mBuffer.resize(mBuffer.size() + availableSpace);
        memcpy(buf, &*begin, availableSpace);

        mNumWritten = availableSpace;
        return availableSpace;
    }


    boost::circular_buffer<uint8_t> mBuffer;
    std::atomic<std::size_t> mNumRead;
    std::atomic<std::size_t> mNumWritten;
};


std::vector<uint8_t> generateTestData()
{
    std::vector<uint8_t> data;
    data.reserve(500);
    while (data.size() != data.capacity())
    {
        data.push_back(data.size());
    }
    return data;
}


class MakeString
{
public:
    template <typename T>
    MakeString & operator<<(const T & datum)
    {
        mBuffer << datum;
        return *this;
    }

    operator std::string() const
    { return mBuffer.str(); }

    std::string str() const
    { return mBuffer.str(); }

private:
    std::ostringstream mBuffer;
};


void print(std::string str)
{
    static std::mutex fMutex;
    std::lock_guard<std::mutex> lock(fMutex);
    std::cout << str << std::endl;
}


int main()
{
    ConcurrentBuffer buf(100);

    auto testData = generateTestData();
    auto numWritten = 0UL;

    std::thread([&] {
        std::vector<uint8_t> receiveData;
        receiveData.reserve(100);
        auto numReceived = 0UL;
        while (numReceived != buf.size()) {
            if (!buf.empty()) {
                numReceived += buf.read(&receiveData[0], receiveData.size() - numReceived);
                print(MakeString() << "numReceived=" << numReceived);
            }
        }
    }).detach();

    while (numWritten != testData.size()) {
        numWritten += buf.write(testData.data(), testData.size());
        print(MakeString() << "numWritten=" << numWritten);
    }
}
