#ifndef MESSAGE_H
#define MESSAGE_H


#include <boost/asio.hpp>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
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


typedef std::function<void(const std::string&)> ClientCallback;


class Message
{
    enum
    {
        cHeaderLength = 8,
        cMaxTotalLength = 100*1024*1024,
        cMaxPayloadLength = cMaxTotalLength - cHeaderLength
    };
    
public:    
    explicit Message(const std::string & str) : mData(std::make_shared<std::string>())
    {
        if (str.size() > cMaxPayloadLength)
        {
            throw std::runtime_error("Message is too long: " + std::to_string(str.size()));
        }
        mData->reserve(cMaxTotalLength);
        mData->resize(cHeaderLength + str.size());
        memcpy(body(), str.data(), str.size());
        encode_header(get_unique_id());
    }
        
    const char * data() const
    {
        return mData->data();
    }
    
    char * data()
    {
        return const_cast<char*>(static_cast<const Message&>(*this).data());
    }
    
    size_t length() const
    {
        return mData->size();
    }

    const char * header() const
    {
        return data();
    }

    char * header()
    {
        return &data()[0];
    }
    
    size_t header_length() const
    {
        return cHeaderLength;
    }

    const char * body() const
    {
        return header() + cHeaderLength;
    }

    char * body()
    {
        return header() + cHeaderLength;
    }

    size_t body_length() const
    {
        return length() - cHeaderLength;
    }
    
    uint32_t get_id() const
    {
        uint32_t id;
        memcpy(&id, data(), sizeof(id));
        return ntohl(id);
    }
    
    void set_id(uint32_t id)
    {
        uint32_t netid = htonl(id);
        memcpy(data(), &netid, sizeof(netid));
    }

    void decode_header()
    {        
        auto body_length = [this]() {
            uint32_t n;
            memcpy(&n, data() + sizeof(uint32_t), sizeof(n));
            return ntohl(n);
        }();
        
        uint32_t new_size = body_length + cHeaderLength;
        if (new_size > cMaxTotalLength)
        {
            throw std::runtime_error("Failed to decode message because it's length exceeds max total length: " + std::to_string(new_size));
        }
        mData->resize(new_size);
    }
    
    void encode_header()
    {
        encode_header(get_id());
    }

    void encode_header(uint32_t id)
    {
        auto netencoded_id = htonl(id);
        memcpy(data(), &netencoded_id, sizeof(netencoded_id));
        
        uint32_t len = htonl(mData->size() - cHeaderLength);
        memcpy(data() + sizeof(uint32_t), &len, sizeof(len));
    }

private:
    static uint32_t get_unique_id()
    {
        static std::atomic<uint32_t> id{};
        return ++id;
    }

    std::shared_ptr<std::string> mData;
};


} // MessageProtocol


#endif // MESSAGE_H
