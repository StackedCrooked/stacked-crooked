#include "Exceptions.h"
#include "Utils.h"
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


    void GetArgs(const std::string & inURI, Args & outArgs)
    {
        std::string name, value;
        bool processingName = true;
        for (size_t idx = inURI.find('?') + 1; idx != inURI.size(); ++idx)
        {
            std::string & str = processingName ? name : value;
            switch(char c = inURI[idx])
            {
                case '=':
                {
                    processingName = false;
                    break;
                }
                case '&':
                {
                    if (!name.empty() && !value.empty())
                    {
                        outArgs.insert(std::make_pair(name, value));
                    }
                    name.clear();
                    value.clear();
                    processingName = true;
                    break;
                }
                default:
                {
                    str += c;
                    break;
                }
            }
        }
        if (!name.empty() && !value.empty())
        {
            outArgs.insert(std::make_pair(name, value));
        }
    }


    const std::string & GetArg(const Args & inArgs, const std::string & inArg)
    {
        Args::const_iterator it = inArgs.find(inArg);
        if (it != inArgs.end())
        {
            return it->second;
        }
        throw MissingArgumentException("Missing argument: " + inArg);
    }

} // namespace HSServer
