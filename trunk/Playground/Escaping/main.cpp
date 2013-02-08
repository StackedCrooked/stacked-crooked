#include "encode.h"
#include <fstream>
#include <iostream>
#include <stdexcept>


struct InvalidUsage : std::runtime_error
{
    InvalidUsage() : std::runtime_error("InvalidUsage") {}
    ~InvalidUsage() throw() {}
};


void run(std::vector<std::string> args)
{
	if (args.size() < 3)
    {
        throw InvalidUsage();
    }

    auto delim = ",";
    auto escape = ".";

    if (args[1] == "encode")
    {
        std::cout << encode_copy(args[2], delim, escape) << std::endl;
    }
    else if (args[1] == "decode")
    {
        std::cout << decode_copy(args[2], delim, escape) << std::endl;
    }
    else
    {
        throw InvalidUsage();
    }
}

int main(int argc, char ** argv)
{
    try
    {
        run(std::vector<std::string>(argv, argv + argc));
    }
    catch (InvalidUsage&)
    {
        std::cerr << "Usage: " << argv[0] << " encode|decode Text" << std::endl;
        return 1;
    }
}
