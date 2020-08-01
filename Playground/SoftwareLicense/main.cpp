#include "Protobuf/license.pb.h"
#include <boost/program_options.hpp>
//#include <cryptopp/sha.h>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>


std::string ExecuteShellCommand(const std::string& cmd)
{
    char buffer[128];

    std::string result;

    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen failed for cmd: " + cmd);
    }

    while (!feof(pipe.get()))
    {
        if (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr)
        {
            result += buffer;
        }
    }

    return result;
}




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
    void add(int64_t value)
    {
        mBuffer += std::to_string(value);
    }

    void add(const std::string& str)
    {
        mBuffer += str;
    }

    uint64_t generate_result()
    {
        std::stringstream ss;
        ss << "echo "
           << '"' << mBuffer << '"'
           << " | shasum -a 256";

        std::string result = "0x" + ExecuteShellCommand(ss.str()).substr(0, 16);

        std::stringstream sss;
        sss << result;

        uint64_t u64 = 0;
        sss >> std::hex >> u64;
        return u64;

#if 0
        std::string result;
        CryptoPP::SHA256 sha;
        sha.Update(reinterpret_cast<const uint8_t*>(mBuffer.data()), mBuffer.size());
        result.resize(sha.DigestSize());
        sha.Final(reinterpret_cast<uint8_t*>(result.data()));
        assert(8 * result.size() == 256); // result should be 256 bit
        return result;
#endif
    }

private:
    std::string mBuffer;
};




////////////////////////////////////////////////////////////////////////////////////////////////////
/// Software License code
////////////////////////////////////////////////////////////////////////////////////////////////////


static constexpr int Version1 = 1;
static constexpr int Version2 = 2;
static constexpr int Version3 = 3;
static constexpr int CurrentVersion = 3;



uint64_t GenerateChecksum(const License& license, int version)
{
    SHA256 shasum;

    shasum.add("Hans en Grietje"); // add salt

    // Version 1 fields:
    shasum.add(license.version());
    shasum.add(license.hardware_id());
    shasum.add(license.num_trunk_ports());
    shasum.add(license.num_nontrunk_ports());

    // Version 2 fields:
    if (version >= Version2)
    {
        shasum.add(license.num_usb_ports());
    }

    // Version 3 fields:
    if (version >= Version3)
    {
        shasum.add(license.num_nbaset_ports());
    }

    // Get checksum
    return shasum.generate_result();
}


struct LicenseConfig
{
    std::string hardware_id;
    int32_t version = CurrentVersion;
    int32_t num_trunk = 0;
    int32_t num_nontrunk = 0;
    int32_t num_usb = 0;
    int32_t num_nbase_t = 0;
};



static uint64_t HardwareIdToUInt64(const std::string& s)
{
    const std::string& command = "echo \"" + s + "\" | perl -pe 's,\\D,,g'";
    const std::string hex = "0x" + ExecuteShellCommand(command);

    std::stringstream ss;
    ss << hex;

    uint64_t u64 = 0;
    ss >> std::hex >> u64;
    return u64;
}



License CreateLicense(const LicenseConfig& config)
{
    License license;

    // Version 1 fields:
    license.set_version(config.version);
    license.set_hardware_id(HardwareIdToUInt64(config.hardware_id));
    license.set_num_trunk_ports(config.num_trunk);
    license.set_num_nontrunk_ports(config.num_nontrunk);
    license.add_checksum(GenerateChecksum(license, Version1));

    // Version 2 fields:
    if (config.version >= Version2)
    {
        license.set_num_usb_ports(config.num_usb);
        license.add_checksum(GenerateChecksum(license, Version2));
    }

    // Version 3 fields:
    if (config.version >= Version3)
    {
        license.set_num_nbaset_ports(config.num_nbase_t);
        license.add_checksum(GenerateChecksum(license, Version3));
    }

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
        std::cerr << "License file validation failed. The file may be corrupted.\n";
        std::exit(-1);
    }

    std::cout << "Checksum OK\n";
}


void ShowLicense(const std::string& filename)
{
    License license = ReadLicenseFromFile(filename);

    std::cout << "=== License Info ===\n";
    std::cout << "  Version: " << license.version() << '\n';
    std::cout << "  HardwareId: " << license.hardware_id() << '\n';

    if (license.version() >= Version1)
    {
        std::cout << "  Number of trunk ports: " << license.num_nontrunk_ports() << '\n';
        std::cout << "  Number of nontrunk ports: " << license.num_trunk_ports() << '\n';
    }

    if (license.version() >= Version2)
    {
        std::cout << "  Number of USB ports: " << license.num_usb_ports() << '\n';
    }

    if (license.version() >= Version3)
    {
        std::cout << "  Number of NBASE-T ports: " << license.num_nbaset_ports() << '\n';
    }

    std::cout << "  Checksum: \n";
    for (auto version = 1; version <= license.version(); ++version)
    {
        std::cout << "    Version " << version << ": ";
        if (version > CurrentVersion)
        {
            std::cout << "(unchecked)\n";
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
    std::string filename;

    try
    {
        LicenseConfig config;

        options.add_options()
            ("filename,F", value(&filename)->required(), "License filename")
            ("version", value(&config.version), "License version number.")
            ("hardwareid", value(&config.hardware_id)->required(), "Hardware identifier that connects the license to a specific machine.")
            ("trunk,T", value(&config.num_trunk)->required(), "Number of trunking ports")
            ("nontrunk,N", value(&config.num_nontrunk)->required(), "Number of nontrunking ports")
            ("usb", value(&config.num_usb), "Number of usb ports")
            ("nbaset", value(&config.num_nbase_t), "Number of NBase-T ports")
                ;

        variables_map vm;
        store(command_line_parser(args).options(options).run(), vm);
        vm.notify();

        if (config.version < Version1 || config.version > CurrentVersion)
        {
            throw std::runtime_error("Invalid version: " + std::to_string(config.version));
        }

        License license = CreateLicense(config);
        WriteLicenseToFile(license, filename);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\nUsage:\n" << options << '\n';
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
        std::cerr << "Error: " << e.what() << '\n';
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
        std::cerr << "Error: " << e.what() << '\n';
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
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}
