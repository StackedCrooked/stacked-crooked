#include <boost/circular_buffer.hpp>
#include <atomic>
#include <iostream>
#include <stdint.h>


struct ConcurrrentBufer
{
    ConcurrrentBufer() :
        mBuffer(),
        mNumRead(0),
        mNumWritten(0)
    {
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

        memcpy(buf, &*begin, numUnread);
        mNumRead += numUnread;
        return numUnread;
    }


    boost::circular_buffer<uint8_t> mBuffer;
    std::atomic<std::size_t> mNumRead;
    std::atomic<std::size_t> mNumWritten;
};


int main()
{
    ConcurrentBuffer buf;
}
