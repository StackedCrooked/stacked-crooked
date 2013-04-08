#ifndef MESSAGE_H
#define MESSAGE_H


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>


class Message
{
public:
    enum { header_length = 4 };
    enum { max_body_length = 512 };
    
    Message(const std::string & str = "")
    {
        if (str.size() > max_body_length + header_length)
        {
            throw std::runtime_error("Message is too long: " + std::to_string(str.size()));
        }
        mData.reserve(max_body_length);
        mData.resize(str.size() + header_length);
        memcpy(body(), str.data(), str.size());
        encode_header();
    }

    const char * data() const
    {
        return mData.data();
    }

    char * data()
    {
        return &mData[0];
    }

    size_t length() const
    {
        return mData.size();
    }

    const char * body() const
    {
        return data() + header_length;
    }

    char * body()
    {
        return data() + header_length;
    }

    size_t body_length() const
    {
        return length() - header_length;
    }

    void resize(uint32_t _new_length)
    {
        uint32_t new_size = _new_length + header_length;
        if (new_size > mData.capacity())
        {
            throw std::runtime_error("New message size exceeds capacity: " + std::to_string(new_size));
        }
        mData.resize(new_size);
        encode_header();
    }

    bool decode_header()
    {
        auto length = [this](){
            uint32_t n;
            memcpy(&n, mData.data(), sizeof(n));
            return ntohl(n);
        }();
        resize(length);
        return true;
    }

    void encode_header()
    {
        uint32_t n = htonl(mData.size() - header_length);
        memcpy(&mData[0], &n, sizeof(n));
    }

private:
    std::string mData;
};


#endif // MESSAGE_H
