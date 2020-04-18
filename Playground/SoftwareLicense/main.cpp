#include "Utils.h" 


enum
{
    Version1 = 1,
    Version2 = 2,
    Version3 = 3,

    CurrentVersion = Version3
};


/**
 * UserFields contains the license fields that we need to validate.
 */
struct UserFields
{
    // Since Version 1:
    uint32_t mNumberOfTrunkingPorts = 0;
    uint32_t mNumberOfNonTrunkingPorts = 0;

    // Since Version 2:
    uint32_t mNumberOfSerialPorts = 0;
    uint32_t mNumberOfBluetoothPorts = 0;

    // Since Version 3:
    uint32_t mNumberOfG5Modules = 0;
};


/**
 * GenerateChecksum: generates the license checksum based on the license fields and a hardware identifier.
 */
uint64_t GenerateChecksum(const UserFields& fields, const std::string& hardware_identifier, int version)
{
    // Adding a salt makes it a little harder to reverse engineer the checksum algorithm.
    static const std::string salt = "Hans en Grietje";

    SHA256 sha256;
    sha256.add(salt);
    sha256.add(hardware_identifier);
    sha256.add(version);

    // Version 1
    sha256.add(fields.mNumberOfTrunkingPorts);
    sha256.add(fields.mNumberOfNonTrunkingPorts);

    if (version >= 2)
    {
        sha256.add(fields.mNumberOfSerialPorts);
        sha256.add(fields.mNumberOfBluetoothPorts);
    }

    if (version >= 3)
    {
        sha256.add(fields.mNumberOfG5Modules);
    }

    return sha256.getLower64Bit();
}


/**
 * LicenseFile: actual contents of the license file
 */
struct LicenseFile
{
    LicenseFile() = default;

    explicit LicenseFile(const UserFields& fields, const std::string& hardware_identifier, int version = CurrentVersion) :
        mVersion(version),
        mChecksum(GenerateChecksum(fields, hardware_identifier, version)),
        mFields(fields)
    {
    }

    static LicenseFile LoadFromString(const std::string& str)
    {
        LicenseFile result;
        memcpy(&result, str.data(), std::min(str.size(), sizeof(result)));
        return result;
    }

    bool validate(const std::string& hardware_identifier) const
    {
        if (mVersion < Version1 || mVersion > CurrentVersion)
        {
            // Unsupported version number.
            return false;
        }

        if (GenerateChecksum(mFields, hardware_identifier, mVersion) != mChecksum)
        {
            // Invalid checksum.
            return false;
        }

        return true;
    }

    std::string serialize() const
    {
        std::string result;
        result.resize(sizeof(*this));
        memcpy(result.data(), this, result.size());
        return result;
    }

    uint64_t mVersion = 0;
    uint64_t mChecksum = 0;
    UserFields mFields;
};


//! Trivially copyable allows us to memcpy the object from and to memory.
static_assert(std::is_trivially_copyable<LicenseFile>::value, "");


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " + std::string(argv[0]) + " hardware_identifier" << std::endl;
        return -1;
    }

    const std::string hardware_identifier = argv[1];

    UserFields fields;
    fields.mNumberOfNonTrunkingPorts = 2;
    fields.mNumberOfTrunkingPorts = 48;
    fields.mNumberOfG5Modules = 8;

    LicenseFile good_license = LicenseFile(fields, hardware_identifier, CurrentVersion);
    assert(good_license.validate(hardware_identifier));
    auto serialized_license = good_license.serialize();

    LicenseFile v2 = LicenseFile(fields, hardware_identifier, Version2);
    assert(v2.validate(hardware_identifier));

    // The G5 field is not part of the checksum for Version 2.
    v2.mFields.mNumberOfG5Modules++;
    assert(v2.validate(hardware_identifier));

    // However, the serial port field is part of the checksum for Version 2.
    v2.mFields.mNumberOfSerialPorts++;
    assert(!v2.validate(hardware_identifier));

    LicenseFile bad_license = good_license;
    assert(bad_license.validate(hardware_identifier));
    bad_license.mFields.mNumberOfTrunkingPorts = 1234;
    assert(!bad_license.validate(hardware_identifier));

    LicenseFile bad = LicenseFile::LoadFromString(bad_license.serialize());
    assert(bad.mFields.mNumberOfTrunkingPorts == bad_license.mFields.mNumberOfTrunkingPorts);
    assert(!bad.validate(hardware_identifier));

    LicenseFile good = LicenseFile::LoadFromString(serialized_license);
    assert(good.validate(hardware_identifier));

    std::cout << "Program finished without errors." << std::endl;

    return 0;
}

