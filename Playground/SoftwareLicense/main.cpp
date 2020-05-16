#include "Protobuf/license.pb.h"
#include <boost/program_options.hpp>
#include <cryptopp/sha.h>
#include <cstdint>
#include <cstring>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>


/**
 * This proof-of-concept provides some basic features for the software license:
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

    std::string generate_result()
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

private:
    CryptoPP::SHA256 mSHA;
    std::string mResult;
    bool mFinalized = false;
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


void UpdateChecksum(SHA256& checksum, const HardwareId& hardware_id, int license_version)
{
    if (license_version >= Version1)
    {
        checksum.add(hardware_id.type());
        checksum.add(hardware_id.value());
    }
}


void UpdateChecksum(SHA256& checksum, const Features& features, int license_version)
{
    if (license_version >= Version1)
    {
        checksum.add(features.num_trunk_ports());
        checksum.add(features.num_nontrunk_ports());
    }

    if (license_version >= Version2)
    {
        checksum.add(features.num_usb_ports());
    }

    if (license_version >= Version3)
    {
        checksum.add(features.num_nbaset_ports());
    }
}


void UpdateChecksum(SHA256& checksum, const Limits& limits, int license_version)
{
    if (license_version >= Version1)
    {
        checksum.add(limits.seconds_assigned());
        checksum.add(limits.seconds_consumed());
        checksum.add(limits.trialperiod_begin());
        checksum.add(limits.trialperiod_end());
    }
}


std::string GenerateSecureHash(const License& license)
{
    SHA256 shasum;

    if (license.version() >= Version1)
    {
        shasum.add("Hans en Grietje"); // add salt
        shasum.add(license.version());

        // Add hardware id fields to checksum
        UpdateChecksum(shasum, license.hardware_id(), license.version());

        // Add license limit fields to checksum
        UpdateChecksum(shasum, license.limits(), license.version());

        // Add license feature fields to checksum
        UpdateChecksum(shasum, license.features(), license.version());
    }

    return shasum.generate_result();
}


void CreateLicense(const std::string& filename, int license_version, HardwareIdType hardware_id_type, const std::string& hardware_id_value, int num_trunk, int num_nontrunk, int num_usb, int num_nbase_t)
{
    License license;
    license.set_version(license_version);
    license.mutable_hardware_id()->set_type(hardware_id_type);
    license.mutable_hardware_id()->set_value(hardware_id_value);

    if (license_version >= Version1)
    {
        license.mutable_features()->set_num_nontrunk_ports(num_nontrunk);
        license.mutable_features()->set_num_trunk_ports(num_trunk);
    }

    if (license_version >= Version2)
    {
        license.mutable_features()->set_num_usb_ports(num_usb);
    }

    if (license_version >= Version3)
    {
        license.mutable_features()->set_num_nbaset_ports(num_nbase_t);
    }

    license.set_secure_hash(GenerateSecureHash(license));


    std::ofstream ofs(filename, std::ios::binary);
    std::string serialized = license.SerializeAsString();
    ofs.write(serialized.data(), serialized.size());
}


License ReadLicenseFromFile(const std::string& filename)
{
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::string raw;
    raw.resize(size);

    if (!ifs.read(raw.data(), size))
    {
        throw std::runtime_error("Failed to read from licensefile.v1");
    }


    License license;
    license.ParseFromString(raw);
    return license;
}


bool CheckLicenseValid(const License& license)
{
    return license.secure_hash() == GenerateSecureHash(license);
}


void VerifyLicense(const std::string& filename)
{
    if (!CheckLicenseValid(ReadLicenseFromFile(filename)))
    {
        std::cerr << "License file validation failed. The file may be corrupted." << std::endl;
        std::exit(-1);
    }
}

struct MACAddress
{
    MACAddress() = default;

    static MACAddress FromString(const std::string& s)
    {
        MACAddress result;
        if (result.assign(s))
        {
            return result;
        }
        throw std::runtime_error("MACAddress: failed to parse " + s);
    }

    static MACAddress FromBinaryString(const std::string& s)
    {
        if (s.size() != 6)
        {
            throw std::runtime_error("Invalid MAC address: expected 6 bytes but got: " + std::to_string((s.size())));
        }

        MACAddress result;
        memcpy(result.data(), s.data(), sizeof(result));
        return result;
    }

    bool assign(std::string string_rep)
    {
        uint32_t part1 = 0;
        uint32_t part2 = 0;
        uint32_t part3 = 0;
        uint32_t part4 = 0;
        uint32_t part5 = 0;
        uint32_t part6 = 0;

        int n = std::sscanf(string_rep.c_str(), "%x%*[-.:]%x%*[-.:]%x%*[-.:]%x%*[-.:]%x%*[-.:]%x", &part1, &part2, &part3, &part4, &part5, &part6);
        if (n != 6)
        {
            return false;
        }

        mArray[0] = part1;
        mArray[1] = part2;
        mArray[2] = part3;
        mArray[3] = part4;
        mArray[4] = part5;
        mArray[5] = part6;
        return true;
    }

    uint8_t* data() { return &mArray[0]; }

    const uint8_t* data() const { return &mArray[0]; }
    std::size_t size() const { return sizeof(mArray) / sizeof(mArray[0]); }

    uint8_t* begin() { return data(); }
    uint8_t* end() { return begin() + size(); }

    const uint8_t* begin() const { return data(); }
    const uint8_t* end() const { return begin() + size(); }

    const char* cstr_begin() const { return reinterpret_cast<const char*>(begin()); }
    const char* cstr_end() const { return reinterpret_cast<const char*>(end()); }

    friend std::ostream& operator<<(std::ostream& os, const MACAddress& mac)
    {
        return os
            << std::hex
            << std::setw(2) << std::setfill('0') << static_cast<int>(mac.mArray[0]) << ":"
            << std::setw(2) << std::setfill('0') << static_cast<int>(mac.mArray[1]) << ":"
            << std::setw(2) << std::setfill('0') << static_cast<int>(mac.mArray[2]) << ":"
            << std::setw(2) << std::setfill('0') << static_cast<int>(mac.mArray[3]) << ":"
            << std::setw(2) << std::setfill('0') << static_cast<int>(mac.mArray[4]) << ":"
            << std::setw(2) << std::setfill('0') << static_cast<int>(mac.mArray[5])
            << std::dec;
    }

    uint8_t mArray[6];
};


void ShowLicense(const std::string& filename)
{
    License license = ReadLicenseFromFile(filename);

    std::cout << "=== License Info ===" << std::endl;

    std::cout << "  Version: " << license.version() << '\n';

    if (license.limits().seconds_assigned())
    {
        std::cout << "  Usage limit: " << license.limits().seconds_assigned() << " seconds\n";
    }
    else
    {
        std::cout << "  Usage limit: unlimited\n";
    }

    if (license.limits().trialperiod_begin())
    {
        std::cout << "  Trial period: " << license.limits().trialperiod_begin() << " - " << license.limits().trialperiod_end() << '\n';
    }

    if (license.version() >= Version1)
    {
        std::cout << "  Number of trunk ports: " << license.features().num_nontrunk_ports() << '\n';
        std::cout << "  Number of nontrunk ports: " << license.features().num_trunk_ports() << '\n';
    }

    if (license.version() >= Version2)
    {
        std::cout << "  Number of USB ports: " << license.features().num_usb_ports() << '\n';
    }

    if (license.version() >= Version3)
    {
        std::cout << "  Number of NBASE-T ports: " << license.features().num_nbaset_ports() << '\n';
    }

    std::cout << "  HardwareId: " << MACAddress::FromBinaryString(license.hardware_id().value()) << '\n';

    if (CheckLicenseValid(license))
    {
        std::cout << "  Secure hash: OK" << '\n';
    }
    else
    {
        std::cout << "  Secure hash: *** INVALID *** " << '\n';
    }
}


void create(const std::vector<std::string>& args)
{
    using namespace boost::program_options;

    options_description options;

    try
    {
        std::string filename;
        HardwareIdType hardwareid_type = MAC_ADDRESS;
        std::string hardwareid_mac;
        int32_t license_version = CurrentVersion;
        int32_t num_trunk = 0;
        int32_t num_nontrunk = 0;
        int32_t num_usb = 0;
        int32_t num_nbase_t = 0;

        options.add_options()
            ("filename,F", value(&filename)->required(), "License filename")
            ("version", value(&license_version), "License version number.")
            ("hardwareid", value(&hardwareid_mac)->required(), "Hardware identifier that connects the license to a specific machine.")
            ("trunk,T", value(&num_trunk)->required(), "Number of trunking ports")
            ("nontrunk,N", value(&num_nontrunk)->required(), "Number of nontrunking ports")
            ("usb", value(&num_usb), "Number of usb ports")
            ("nbaset", value(&num_nbase_t), "Number of NBase-T ports")
                ;

        variables_map vm;
        store(command_line_parser(args).options(options).run(), vm);
        vm.notify();

        if (license_version < Version1 || license_version > CurrentVersion)
        {
            throw std::runtime_error("Invalid version: " + std::to_string(license_version));
        }

        auto mac = MACAddress::FromString(hardwareid_mac);
        std::string binary_mac(mac.begin(), mac.end());

        CreateLicense(filename, license_version, hardwareid_type, binary_mac, num_trunk, num_nontrunk, num_usb, num_nbase_t);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\nUsage:\n" << options << std::endl;
        std::exit(-1);
    }
}


void check(const std::vector<std::string>& args)
{
    try
    {
        if (args.size() != 1)
        {
            throw std::runtime_error("Usage: license check FileName");
        }

        VerifyLicense(args[0]);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::exit(-1);
    }
}


void show(const std::vector<std::string>& args)
{
    try
    {
        if (args.size() != 1)
        {
            throw std::runtime_error("Usage: license check FileName");
        }

        ShowLicense(args[0]);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::exit(-1);
    }
}


void run(int argc, char** argv)
{
    if (!strcmp(argv[0], "create"))
    {
        create(std::vector<std::string>(argv + 1, argv + argc));
    }
    else if (!strcmp(argv[0], "check"))
    {
        check(std::vector<std::string>(argv + 1, argv + argc));
    }
    else if (!strcmp(argv[0], "show"))
    {
        show(std::vector<std::string>(argv + 1, argv + argc));
    }
    else
    {
        throw std::runtime_error("Invalid command: " + std::string(argv[0]));
    }
}

int main(int argc, char** argv)
{
    try
    {
        run(argc - 1, argv + 1);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
