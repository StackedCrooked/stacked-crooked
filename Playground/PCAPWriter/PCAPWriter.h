#ifndef PCAPWRITER_H
#define PCAPWRITER_H


#include <boost/scoped_ptr.hpp>
#include <string>


class PCAPWriter
{
public:
    PCAPWriter(const std::string & inOutputFile);

    ~PCAPWriter();

    void push_back(const uint8_t* bytes, uint32_t len);

private:
    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


#endif // PCAPWRITER_H
