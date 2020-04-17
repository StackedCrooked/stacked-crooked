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
inline T Decode(const uint8_t* bytes)
{
    static_assert(std::is_pod<T>::value, "");

    T result;
    memcpy(&result, bytes, sizeof(result));
    return result;
}


// Wrapper around a network encoded 32-bit unsigned integer.
struct Net32
{
    void operator=(uint32_t value)
    {
        mValue = htonl(value);
    }

    friend bool operator==(Net32 lhs, Net32 rhs)
    {
        return lhs.mValue == rhs.mValue;
    }

    uint32_t mValue;
};


//! Helper for generating SHA-256 checksums.
struct ChecksumGenerator
{
    ChecksumGenerator(const std::string& salt) :
        mSHA()
    {
        add(salt);
    }

    void add(Net32 value)
    {
        mSHA.Update(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
    }

    void add(const std::string& str)
    {
        mSHA.Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
    }

    void finalize(Net32& result)
    {
        auto vec = finalize();
        memcpy(&result, vec.data() + vec.size() - sizeof(result), sizeof(result));
    }

    std::vector<uint8_t> finalize()
    {
        std::vector<uint8_t> result(mSHA.DigestSize());
        mSHA.Final(reinterpret_cast<uint8_t*>(result.data()));
        assert(8 * result.size() == 256); // result should be 256 bit 
        return result;
    }

    CryptoPP::SHA256 mSHA;
};


//! License binary format
struct License
{
    Net32 calculate_checksum(const std::string& hardware_id)
    {
        Net32 result;

        ChecksumGenerator c(hardware_id);
        c.add(major_version);
        c.add(minor_version);
        c.add(num_trunks);
        c.add(num_nontrunks);
        c.add(num_nbase_t);

        std::vector<uint8_t> bytes = c.finalize();

        memcpy(&result, bytes.data() + bytes.size() - sizeof(result), sizeof(result));

        return result;
    }

    bool validate_checksum(const std::string& hardware_id)
    {
        return calculate_checksum(hardware_id) == checksum;
    }

    std::vector<uint8_t> serialize() const
    {
        std::vector<uint8_t> result(sizeof(*this));
        memcpy(result.data(), this, result.size());
        return result;
    }

    static License parse(const std::vector<uint8_t>& vec)
    {
        License result;
        memcpy(&result, vec.data(), std::min(vec.size(), sizeof(result)));
        return result;
    }

    Net32 major_version{};
    Net32 minor_version{};
    Net32 checksum{};

    Net32 num_trunks{};
    Net32 num_nontrunks{};
    Net32 num_nbase_t{};
};


static_assert(std::is_trivially_copyable<License>::value, "");


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " + std::string(argv[0]) + " HARDWARE_ID" << std::endl;
        return -1;
    }

    static const std::string salt = "Excentis - Gildestraat 8 - 9000 Gent";

    // Hardware ID is salted with secret string.
    std::string hardware_id = salt + argv[1];

    License good_license = License();
    good_license.num_trunks = 48;
    good_license.num_nontrunks = 2;
    good_license.checksum = good_license.calculate_checksum(hardware_id);
    auto serialized_license = good_license.serialize();

    License bad_license = good_license;
    bad_license.num_trunks = 1234;
    assert(!bad_license.validate_checksum(hardware_id));

    License bad = License::parse(bad_license.serialize());
    assert(bad.num_trunks == bad_license.num_trunks);
    assert(!bad.validate_checksum(hardware_id));

    License good = License::parse(serialized_license);
    assert(good.validate_checksum(hardware_id));

    return 0;
}

