#include "FileUtils.h"
#include <fstream>
#include <streambuf>


namespace HSServer
{

    void ReadEntireFile(const std::string & inPath, std::string & str)
    {
        std::ifstream t(inPath.c_str());

        t.seekg(0, std::ios::end);   
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    }

} // namespace HSServer
