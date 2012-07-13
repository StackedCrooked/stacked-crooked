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
#include <iostream>
#include <memory>
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

    std::ofstream out(inTargetPath.c_str(), std::ios::binary);
    Poco::InflatingOutputStream outInflate(out, Poco::InflatingStreamBuf::STREAM_ZLIB);

    std::streamsize s = 0;
    while (auto n = Poco::StreamCopier::copyStream(*pStr.get(), outInflate))
    {
        s += n;
        std::cout << s << " bytes written (" << n << ")" << std::endl;
    }
    std::cout << std::endl;
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
