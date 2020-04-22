#include <cryptopp/sha.h>
#include <cstdint>
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
    Version3 = 3,

    CurrentVersion = Version3
};


/**
 * LicenseFields contains the license fields that need to be validated.
 *
 * To enable backwards compatibility:
 *   - new fields are always added at the end
 *   - existing fields are never changed or removed
 */
struct LicenseFields
{
    uint32_t version = CurrentVersion;

    uint32_t numberOfTrunkingPorts = 0;
    uint32_t numberOfNonTrunkingPorts = 0;

    // Since Version 2:
    uint32_t numberOfSerialPorts = 0;
    uint32_t numberOfBluetoothPorts = 0;

    // Since Version 3:
    uint32_t numberOfG5Modules = 0;
};


/**
 * Generates a secure hash of the license fields and a hardware identifier.
 */
uint64_t CalculateChecksum(const LicenseFields& fields, const std::string& hardware_identifier)
{
    SHA256 sha256;
    sha256.add("Hans en Grietje" + hardware_identifier); // use "salted" hardware identifier
    sha256.add(fields.version);
    sha256.add(fields.numberOfTrunkingPorts);
    sha256.add(fields.numberOfNonTrunkingPorts);

    if (fields.version >= 2)                                                                        // Backwards compatibility for old licenses
    {
        sha256.add(fields.numberOfSerialPorts);
        sha256.add(fields.numberOfBluetoothPorts);
    }

    if (fields.version >= 3)
    {
        sha256.add(fields.numberOfG5Modules);
    }

    return sha256.getLower64Bit();
}


/**
 * LicenseFile: actual contents of the license file
 */
struct LicenseFile
{
    LicenseFile() = default;

    explicit LicenseFile(const LicenseFields& fields, const std::string& hardware_identifier) :
        mChecksum(CalculateChecksum(fields, hardware_identifier)),
        mFields(fields)
    {
    }

    bool validate(const std::string& hardware_identifier) const
    {
        if (CalculateChecksum(mFields, hardware_identifier) != mChecksum)
        {
            // Invalid checksum.
            return false;
        }

        return true;
    }

    uint64_t mChecksum = 0;
    LicenseFields mFields;
};


std::string serialize(const LicenseFile& license_file)
{
    std::string result;
    result.resize(sizeof(license_file));
    memcpy(result.data(), &license_file, result.size());
    return result;
}


LicenseFile parse(const std::string& str)
{
    LicenseFile result;
    memcpy(&result, str.data(), std::min(str.size(), sizeof(result)));
    return result;
}


//! Check that we can use memcpy safely.
static_assert(std::is_trivially_copyable<LicenseFile>::value, "");


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
     *   - MAC address of management port: cat /sys/class/net/man0/address
     *   - ...
     */
    const std::string hardware_identifier = argv[1];

    LicenseFields fields;
    fields.numberOfNonTrunkingPorts = 2;
    fields.numberOfTrunkingPorts = 48;
    fields.numberOfG5Modules = 8;

    LicenseFile license(fields, hardware_identifier);
    assert(license.validate(hardware_identifier));

    std::cout << "Program finished without errors." << std::endl;

    return 0;
}
