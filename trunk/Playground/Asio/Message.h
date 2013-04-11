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
    enum { header_length = 8 };
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
    
    uint32_t get_id() const
    {
        uint32_t id;
        memcpy(&id, mData.data(), sizeof(id));
        return ntohl(id);
    }

    void decode_header()
    {        
        auto body_length = [this]() {
            uint32_t n;
            memcpy(&n, mData.data() + sizeof(uint32_t), sizeof(n));
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
        uint32_t id = get_unique_id();
        memcpy(&mData[0], &id, sizeof(id));
        
        uint32_t len = htonl(mData.size() - header_length);
        memcpy(&mData[sizeof(uint32_t)], &len, sizeof(len));
    }

private:
    static uint32_t get_unique_id()
    {
        static uint32_t id = 0;
        return id++;
    }

    std::string mData;
};


} // MessageProtocol


#endif // MESSAGE_H
