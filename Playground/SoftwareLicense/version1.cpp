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
 */


//! Helper for generating SHA-256 checksums.
struct SHA256
{
    void add(int64_t value)
    {
        mBuffer += std::to_string(value);
    }

    void add(const std::string& str)
    {
        mBuffer += str;
    }

    std::string generate_result()
    {
        std::string result;

        CryptoPP::SHA256 sha;
        sha.Update(reinterpret_cast<const uint8_t*>(mBuffer.data()), mBuffer.size());
        result.resize(sha.DigestSize());
        sha.Final(reinterpret_cast<uint8_t*>(result.data()));

        assert(8 * result.size() == 256); // result should be 256 bit

        return result;
    }

private:
    std::string mBuffer;
};




////////////////////////////////////////////////////////////////////////////////////////////////////
/// Software License code
////////////////////////////////////////////////////////////////////////////////////////////////////


static constexpr int Version1 = 1;
static constexpr int CurrentVersion = 1;



std::string GenerateChecksum(const License& license, int version)
{
    SHA256 shasum;

    shasum.add("Gildestraat 8, 9000 Gent"); // add salt

    // Version 1 fields:
    shasum.add(license.version());
    shasum.add(license.hardware_id());
    shasum.add(license.num_trunk_ports());
    shasum.add(license.num_nontrunk_ports());

    // Get checksum
    return shasum.generate_result();
}


License CreateLicense(int version, const std::string& hardware_id, int num_trunk, int num_nontrunk, int num_usb, int num_nbase_t)
{
    License license;

    // Version 1 fields:
    license.set_version(version);
    license.set_hardware_id(hardware_id);
    license.set_num_trunk_ports(num_trunk);
    license.set_num_nontrunk_ports(num_nontrunk);
    license.add_checksum(GenerateChecksum(license, Version1));

    return license;
}


void WriteLicenseToFile(const License& license, const std::string& filename)
{
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


bool CheckLicenseValid(const License& license, int max_version)
{
    // Validate all checksums until the maximum common checksum.
    for (auto version = 1; version <= std::min(max_version, license.version()); ++version)
    {
        if (license.checksum(version - 1) != GenerateChecksum(license, version))
        {
            return false;
        }
    }

    return true;
}


void VerifyLicenseIntegrity(const std::string& filename)
{
    License license = ReadLicenseFromFile(filename);

    if (!CheckLicenseValid(license, std::min(license.version(), CurrentVersion)))
    {
        std::cerr << "License file validation failed. The file may be corrupted." << std::endl;
        std::exit(-1);
    }

    std::cout << "Checksum OK\n";
}


void ShowLicense(const std::string& filename)
{
    License license = ReadLicenseFromFile(filename);

    std::cout << "=== License Info ===" << std::endl;
    std::cout << "  Version: " << license.version() << '\n';
    std::cout << "  HardwareId: " << license.hardware_id() << '\n';

    if (license.version() >= Version1)
    {
        std::cout << "  Number of trunk ports: " << license.num_nontrunk_ports() << '\n';
        std::cout << "  Number of nontrunk ports: " << license.num_trunk_ports() << '\n';
    }

    std::cout << "  Checksum: " << std::endl;
    for (auto version = 1; version <= license.version(); ++version)
    {
        std::cout << "    Version " << version << ": ";
        if (version > CurrentVersion)
        {
            std::cout << "(unchecked)" << std::endl;
            continue;
        }

        if (CheckLicenseValid(license, CurrentVersion))
        {
            std::cout << "OK\n";
        }
        else
        {
            std::cout << "NOT OK\n";
        }
    }
}


void create(const std::vector<std::string>& args)
{
    using namespace boost::program_options;

    options_description options;

    try
    {
        std::string filename;
        std::string hardwareid;
        int32_t license_version = CurrentVersion;
        int32_t num_trunk = 0;
        int32_t num_nontrunk = 0;
        int32_t num_usb = 0;
        int32_t num_nbase_t = 0;

        options.add_options()
            ("filename,F", value(&filename)->required(), "License filename")
            ("version", value(&license_version), "License version number.")
            ("hardwareid", value(&hardwareid)->required(), "Hardware identifier that connects the license to a specific machine.")
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

        License license = CreateLicense(license_version, hardwareid, num_trunk, num_nontrunk, num_usb, num_nbase_t);
        WriteLicenseToFile(license, filename);
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

        VerifyLicenseIntegrity(args[0]);
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
        if (argc < 2)
        {
            throw std::runtime_error("Action required: create, show, check");
        }
        run(argc - 1, argv + 1);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
