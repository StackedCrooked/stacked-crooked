#include "Protobuf/license.pb.h"
#include <cryptopp/sha.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>


/**
 * This proof-of-concept provides 3 core features for the software license:
 *
 *  (1) License integrity check: detect invalid changes in the license file
 *
 *  (2) Hardware check: prevents using a license on different machines
 *
 *  (3) Backwards compatibility: enable validation old license version
 *
 *
 * NOTE: below code should be considered as pseudo-code.
 */


//! Helper for generating SHA-256 checksums.
struct SHA256
{
    void add(int32_t value)
    {
        assert(!mFinalized);
        mSHA.Update(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
    }

    void add(int64_t value)
    {
        assert(!mFinalized);
        mSHA.Update(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
    }

    void add(const std::string& str)
    {
        assert(!mFinalized);
        mSHA.Update(reinterpret_cast<const uint8_t*>(str.data()), str.size());
    }

    std::vector<uint8_t> getResult()
    {
        if (!mFinalized)
        {
            mResult.resize(mSHA.DigestSize());
            mSHA.Final(reinterpret_cast<uint8_t*>(mResult.data()));
            assert(8 * mResult.size() == 256); // result should be 256 bit
            mFinalized = true;
        }
        return mResult;
    }

    uint64_t getLower64Bit()
    {
        std::vector<uint8_t> bytes = getResult();
        uint64_t result = 0;
        memcpy(&result, bytes.data(), sizeof(result));
        return result;
    }

private:
    CryptoPP::SHA256 mSHA;
    std::vector<uint8_t> mResult;
    bool mFinalized = false;
};



////////////////////////////////////////////////////////////////////////////////////////////////////
/// Software License code
////////////////////////////////////////////////////////////////////////////////////////////////////


enum Version
{
    Version1 = 1,
    Version2 = 2,
    Version3 = 3
};


static constexpr Version current_version = Version3;


uint64_t CalculateChecksum(const License& license, const std::string& hardware_id)
{
    SHA256 checksum;
    checksum.add(hardware_id);

    checksum.add(license.version());

    if (license.version() >= Version1)
    {
        checksum.add(license.num_trunk_ports());
        checksum.add(license.num_nontrunk_ports());
    }

    if (license.version() >= Version2)
    {
        checksum.add(license.num_usb_ports());
    }

    if (license.version() >= Version3)
    {
        checksum.add(license.num_nbaset_ports());
    }

    return checksum.getLower64Bit();
}


License GenerateLicense_v1(const std::string& hardware_identifier, int num_nontrunk, int num_trunk)
{
    License license;
    license.set_version(Version1);
    license.set_num_nontrunk_ports(num_nontrunk);
    license.set_num_trunk_ports(num_trunk);
    license.set_checksum(CalculateChecksum(license, hardware_identifier));
    return license;
}


License GenerateLicense_v2(const std::string& hardware_identifier, int num_nontrunk, int num_trunk, int num_usb)
{
    License license;
    license.set_version(Version1);
    license.set_num_nontrunk_ports(num_nontrunk);
    license.set_num_trunk_ports(num_trunk);
    license.set_num_usb_ports(num_usb);
    license.set_checksum(CalculateChecksum(license, hardware_identifier));
    return license;
}


void GenerateLicense(Version version, const std::string& hardware_identifier, int num_nontrunk, int num_trunk, int num_usb, int num_nbase_t)
{
    License license;
    license.set_version(version);

    if (version >= Version1)
    {
        license.set_num_nontrunk_ports(num_nontrunk);
        license.set_num_trunk_ports(num_trunk);
    }

    if (version >= Version2)
    {
        license.set_num_usb_ports(num_usb);
    }

    if (version >= Version3)
    {
        license.set_num_nbaset_ports(num_nbase_t);
    }

    license.set_checksum(CalculateChecksum(license, hardware_identifier));
}


void CreateLicense(const std::string& filename, const std::string& hardware_identifier)
{
    std::cout << "Creating license " << filename << " using hardware id " << hardware_identifier << std::endl;


    License license_v1 = GenerateLicense_v1(hardware_identifier, 1, 48);
    license_v1.SerializeAsString();


    License license;
    license.set_version(current_version);
    license.set_num_nontrunk_ports(2);
    license.set_num_trunk_ports(48);
    license.set_checksum(CalculateChecksum(license, hardware_identifier));


    {
        std::ofstream ofs(filename, std::ios::binary);
        std::string serialized = license.SerializeAsString();
        ofs.write(serialized.data(), serialized.size());
        std::cout << "Written " << filename << std::endl;
    }

}


int usage_create(const char* program_name)
{
    std::cerr << "Usage: " << program_name << " create <filename> <hardwareid>" << std::endl;
    return -1;
}


int main(int argc, char** argv)
{
    // Hardware identifier must passed as command line argument.
    if (argc < 3)
    {
        std::cerr
            << "Usage: \n"
            << "  " << argv[0] << " create <filename> <hardwareid>\n"
            << "  " << argv[0] << " check <filename>\n"
            ;
        return -1;
    }

    if (!strcmp(argv[1], "create"))
    {
        if (argc != 4)
        {
            return usage_create(argv[0]);
        }

        const std::string& filename = argv[2];
        const std::string& hardware_identifier = argv[3];

        CreateLicense(filename, hardware_identifier);
        return 0;
    }

    if (!strcmp(argv[1], "check"))
    {
        std::cout << "Checking license" << std::endl;
        return -1;
    }


    return 0;
}
