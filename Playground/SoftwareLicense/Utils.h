#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <arpa/inet.h>




//! Decode binary data to POD object.
template<typename T>
inline T Decode(const void* bytes)
{
    static_assert(std::is_trivially_copyable<T>::value, "");

    T result;
    memcpy(&result, bytes, sizeof(result));
    return result;
}


//! Helper for generating SHA-256 checksums.
struct SHA256
{
    void add(uint32_t value)
    {
        mSHA.Update(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
    }

    void add(const std::string& str)
    {
        mSHA.Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
    }

    std::vector<uint8_t> getBytes()
    {
        std::vector<uint8_t> result(mSHA.DigestSize());
        mSHA.Final(reinterpret_cast<uint8_t*>(result.data()));
        assert(8 * result.size() == 256); // result should be 256 bit
        return result;
    }

    uint64_t get()
    {
        std::vector<uint8_t> bytes = getBytes();
        return Decode<uint64_t>(bytes.data() + bytes.size() - sizeof(uint64_t));
    }

    CryptoPP::SHA256 mSHA;
};
