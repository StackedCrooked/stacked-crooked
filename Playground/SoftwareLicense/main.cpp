#include "Utils.h"

//! License binary format
struct License
{
    uint64_t calculate_checksum(const std::string& hardware_id)
    {
        SHA256 sha256;
        sha256.add(hardware_id);
        sha256.add(mMajorVersion);
        sha256.add(mMinorVersion);
        sha256.add(mNumTrunkPorts);
        sha256.add(mNumNonTrunkPorts);
        sha256.add(mNumNBaseTPorts);

        // Extract final 64 bits from the checksum string.
        std::vector<uint8_t> bytes = sha256.get();
        return Decode<uint64_t>(bytes.data() + bytes.size() - sizeof(uint64_t));
    }

    bool validate_checksum(const std::string& hardware_id)
    {
        return calculate_checksum(hardware_id) == mChecksum;
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

    uint32_t mMajorVersion{};
    uint32_t mMinorVersion{};
    uint64_t mChecksum{};

    uint32_t mNumTrunkPorts{};
    uint32_t mNumNonTrunkPorts{};
    uint32_t mNumNBaseTPorts{};
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
    good_license.mNumTrunkPorts = 48;
    good_license.mNumNonTrunkPorts = 2;
    good_license.mChecksum = good_license.calculate_checksum(hardware_id);
    auto serialized_license = good_license.serialize();

    License bad_license = good_license;
    bad_license.mNumTrunkPorts = 1234;
    assert(!bad_license.validate_checksum(hardware_id));

    License bad = License::parse(bad_license.serialize());
    assert(bad.mNumTrunkPorts == bad_license.mNumTrunkPorts);
    assert(!bad.validate_checksum(hardware_id));

    License good = License::parse(serialized_license);
    assert(good.validate_checksum(hardware_id));

    std::cout << "Program finished without errors." << std::endl;

    return 0;
}

