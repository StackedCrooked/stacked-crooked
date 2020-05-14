#include "Protobuf/license.pb.h"
#include <boost/program_options.hpp>
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
    Version3 = 3,
    CurrentVersion = Version3
};


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


void CreateLicense(const std::string& filename, const std::string& hardware_identifier, int num_trunk, int num_nontrunk)
{
    License license;
    license.set_version(CurrentVersion);
    license.set_num_nontrunk_ports(num_nontrunk);
    license.set_num_trunk_ports(num_trunk);
    license.set_checksum(CalculateChecksum(license, hardware_identifier));

    std::ofstream ofs(filename, std::ios::binary);
    std::string serialized = license.SerializeAsString();
    ofs.write(serialized.data(), serialized.size());
}


void VerifyLicense(const std::string& filename, const std::string& hardware_id)
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

    auto checksum = CalculateChecksum(license, hardware_id);

    if (checksum != license.checksum())
    {
        std::cerr << "Invalid license checksum. The hardware identifier doesn't match or license file is corrupted." << std::endl;
        std::exit(-1);
    }
}


void create(const std::vector<std::string>& args)
{
    using namespace boost::program_options;

    options_description options;

    try
    {
        std::string filename;
        std::string hardware_id;
        int32_t num_trunk = 0;
        int32_t num_nontrunk = 0;

        options.add_options()
            ("filename,F", value(&filename)->required(), "License filename")
            ("hardware_id,H", value(&hardware_id)->required(), "Hardware identifier that connects the license to a specific machine.")
            ("trunk,T", value(&num_trunk)->required(), "Number of trunking ports")
            ("nontrunk,N", value(&num_nontrunk)->required(), "Number of nontrunking ports");

        variables_map vm;
        store(command_line_parser(args).options(options).run(), vm);
        vm.notify();
        CreateLicense(filename, hardware_id, num_trunk, num_nontrunk);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n\nUsage:\n" << options << std::endl;
        std::exit(-1);
    }
}


void check(const std::vector<std::string>& args)
{
    using namespace boost::program_options;

    options_description options;

    try
    {
        std::string filename;
        std::string hardware_id;

        options.add_options()
            ("filename,F", value(&filename)->required(), "License filename")
            ("hardware_id,H", value(&hardware_id)->required(), "Hardware identifier that connects the license to a specific machine.");

        variables_map vm;
        store(command_line_parser(args).options(options).run(), vm);
        vm.notify();
        VerifyLicense(filename, hardware_id);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n\nUsage:\n" << options << std::endl;
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
