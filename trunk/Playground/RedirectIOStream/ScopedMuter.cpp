#include "ScopedMuter.h"
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/noncopyable.hpp>
#include <iostream>


struct ScopedMuter::FakeSink
{
    // Indicate that we work with UTF8 strings.
    typedef char char_type;

    // Indicate that this is an output device.
    typedef boost::iostreams::sink_tag category;

    std::streamsize write(const char*, std::streamsize n)
    {
        // Do nothing.
        return n;
    }
};


struct ScopedMuter::Impl : boost::noncopyable
{
    Impl(std::ostream & inOutStream) :
        mStream(inOutStream),
        mNewBuffer(),
        mOldBuffer(0)
    {
        mNewBuffer.open(FakeSink());
        mOldBuffer = mStream.rdbuf(&mNewBuffer);
    }

    ~Impl()
    {
        mStream.rdbuf(mOldBuffer);
    }

    std::ostream & mStream;
    boost::iostreams::stream_buffer<FakeSink> mNewBuffer;
    std::streambuf * mOldBuffer;
};


ScopedMuter::ScopedMuter(std::ostream & inOutStream) :
    mImpl(new Impl(inOutStream))
{
}


ScopedMuter::~ScopedMuter()
{
    delete mImpl;
}

