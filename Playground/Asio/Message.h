#ifndef MESSAGE_H
#define MESSAGE_H


#include <boost/asio.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>


#include <iostream>


namespace MessageProtocol {


boost::asio::io_service & get_io_service()
{
    static boost::asio::io_service result;
    return result;
}


typedef std::function<std::string(const std::string&)> Callback;


class Message
{
public:
    enum { header_length = 4 };
    enum { max_body_length = 100*1024*1024 };
    
    Message(const std::string & str = "")
    {
        if (str.size() > max_body_length + header_length)
        {
            throw std::runtime_error("Message is too long: " + std::to_string(str.size()));
        }
        mData.reserve(header_length + max_body_length);
        mData.resize(header_length + str.size());
        memcpy(body(), str.data(), str.size());
        encode_header();
    }

    const char * header() const
    {
        return mData.data();
    }

    char * header()
    {
        return &mData[0];
    }

    size_t length() const
    {
        return mData.size();
    }

    const char * body() const
    {
        return header() + header_length;
    }

    char * body()
    {
        return header() + header_length;
    }

    size_t body_length() const
    {
        return length() - header_length;
    }

    void decode_header()
    {
        auto body_length = [this](){
            uint32_t n;
            memcpy(&n, mData.data(), sizeof(n));
            return ntohl(n);
        }();
        uint32_t new_size = body_length + header_length;
        if (new_size > mData.capacity())
        {
            throw std::runtime_error("New message size exceeds capacity: " + std::to_string(new_size));
        }
        mData.resize(new_size);
    }

    void encode_header()
    {
        uint32_t n = htonl(mData.size() - header_length);
        memcpy(&mData[0], &n, sizeof(n));
    }

private:
    std::string mData;
};


} // MessageProtocol


#endif // MESSAGE_H
