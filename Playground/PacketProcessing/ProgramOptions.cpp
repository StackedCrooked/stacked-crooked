#include "ProgramOptions.h"
#include <boost/program_options.hpp>
#include <iostream>


namespace po = boost::program_options;


int initialize_program_options(int argc, char** argv, Options& options)
{
    po::options_description desc("Allowed options");

    desc.add_options()
        ("help", "produce help message")
        ("filter,F", po::value<std::string>(), "filter implementation")
        //("prefetch,P", po::value<int>(), "number of packets to prefetch")
        //("packet_size", po::value<int>(), "packet size")
    ;

    po::variables_map vm;

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    if (vm.count("filter"))
    {
        auto filter = vm["filter"].as<std::string>();

        if (filter == "BPF")
        {
            options.filter = Filter::BPF;
        }
        else if (filter == "Native")
        {
            options.filter = Filter::Native;
        }
        else if (filter == "Mask")
        {
            options.filter = Filter::Mask;
        }
        else if (filter == "Vector")
        {
            options.filter = Filter::Vector;
        }
    }

    #if 0
    if (vm.count("prefetch"))
    {
        auto prefetch_value = vm["prefetch"].as<int>();
        if (prefetch_value < 0)
        {
            std::cerr << "bad prefetch value." << std::endl;
            return 1;
        }

        if (prefetch_value > 16)
        {
            std::cerr << "exceeded maximum prefetch value" << std::endl;
            return 1;
        }

        options.prefetch = prefetch_value;
    }
    #endif

    return 0;
}


