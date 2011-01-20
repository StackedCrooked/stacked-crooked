#include "ScopedMuter.h"
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/noncopyable.hpp>
#include <iostream>


struct ScopedMuter::FakeSink : public boost::iostreams::sink
{
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

