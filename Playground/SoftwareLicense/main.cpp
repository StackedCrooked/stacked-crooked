#include <cryptopp/sha.h>
#include "Protobuf/license.pb.h"
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

    uint64_t getLower64Bit()
    {
        std::vector<uint8_t> bytes = getBytes();

        uint64_t result = 0;
        memcpy(&result, bytes.data(), sizeof(result));
        return result;
    }

    CryptoPP::SHA256 mSHA;
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
    SHA256 sha;
    sha.add("Hans en Grietje" + hardware_id);

    sha.add(license.version());

    if (license.version() >= Version1)
    {
        sha.add(license.num_trunk_ports());
        sha.add(license.num_nontrunk_ports());
    }

    if (license.version() >= Version2)
    {
        sha.add(license.num_usb_ports());
    }

    if (license.version() >= Version3)
    {
        sha.add(license.num_nbaset_ports());
    }

    return sha.getLower64Bit();
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




int main(int argc, char** argv)
{
    // Hardware identifier must passed as command line argument.
    if (argc != 2)
    {
        std::cerr << "Usage: " + std::string(argv[0]) + " hardware_identifier" << std::endl;
        return -1;
    }


    /**
     * Hardware identifier candidates:
     *   - Motherboard serial number: cat /sys/class/dmi/id/board_serial
     *   - MAC address of management port:cat /sys/class/net/man0/address
     *   - ...
     */
    const std::string hardware_identifier = argv[1];


    License license_v1 = GenerateLicense_v1(hardware_identifier, 1, 48);
    license_v1.SerializeAsString();


    License license;
    license.set_version(current_version);
    license.set_num_nontrunk_ports(2);
    license.set_num_trunk_ports(48);

    license.set_checksum(CalculateChecksum(license, hardware_identifier));

    std::string serialized = license.SerializeAsString();
    std::cout << "serialized.size=" << serialized.size() << std::endl;


    {
        std::ofstream ofs("license.v1", std::ios::binary);
        ofs.write(serialized.data(), serialized.size());
    }

    std::ifstream ifs("license.v1", std::ios::binary | std::ios::ate);
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::string raw;
    raw.resize(size);

    if (!ifs.read(raw.data(), size))
    {
        throw std::runtime_error("Failed to read from licensefile.v1");
    }

    License deserialized;
    deserialized.ParseFromString(raw);

    std::cout << "deserialized.version()=" << deserialized.version() << std::endl;

    auto checksum = CalculateChecksum(deserialized, hardware_identifier);
    std::cout << "deserialized.checksum=" << deserialized.checksum() << " verify=" << checksum << std::endl;

    std::cout << "raw.size=" << raw.size() << std::endl;


    std::cout << "Program finished without errors." << std::endl;

    return 0;
}
