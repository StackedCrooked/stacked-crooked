#include "Poco/Foundation.h"
#include "Poco/InflatingStream.h"
#include "Poco/Path.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Zip/Decompress.h"
#include <boost/scoped_ptr.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>


struct Register
{
    Register()
    {
        Poco::Net::HTTPStreamFactory::registerFactory();
    }
};

void DownloadAndDecompressFile(const std::string & inUrl, const std::string & inTargetPath)
{
    static Register fRegister;


    Poco::URI uri(inUrl);
    boost::scoped_ptr<std::istream> pStr(Poco::URIStreamOpener::defaultOpener().open(uri));

    std::string dataString;

    while (true)
    {
        int c = pStr->get();
        if (c != -1)
        {
           dataString += (char) c;
           std::cout << "\rsize: " << dataString.size();
        }
        else
        {
            break;
        }
    }
    std::cout << std::endl;

	// Print output (debugging)
    for (char c : dataString)
    {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << int(uint8_t(c)) << " (" << std::dec << int(uint8_t(c)) << ") ";
    }
    std::cout << std::endl;

    std::istringstream is(dataString);
    Poco::Zip::Decompress(is, inTargetPath);
}


int main(int argc, char ** argv)
{
    std::vector<std::string> args(argv, argv + argc);
    if (args.size() < 3)
    {
        throw std::runtime_error("Invalid arg count. Usage: UnzipURL URL TargetPath");
    }

    std::string url = args.at(1);
    std::cout << "url: " << url << std::endl;

    std::string targetPath = ".";
    if (args.size() >= 3)
    {
        targetPath = args.at(2);
    }
    std::cout << "targetPath: " << targetPath << std::endl;

	DownloadAndDecompressFile(url, targetPath);
    std::cout << "Finished." << std::endl;
}
