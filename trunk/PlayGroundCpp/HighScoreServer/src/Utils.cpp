#include "Exceptions.h"
#include "Utils.h"
#include "Poco/URI.h"
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


    std::string MakeHTML(const std::string & inHTMLElement, const std::string & inText)
    {
        return "<" + inHTMLElement + ">" + inText + "</" + inHTMLElement + ">";
    }


    std::ostream & StreamHTML(const std::string & inHTMLElement, const std::string & inText, std::ostream & ostr)
    {
        return ostr << "<" << inHTMLElement << ">" << inText << "</" << inHTMLElement << ">";
    }
    
    
    std::ostream & StreamHTML(const std::string & inHTMLElement,
                              const StreamFunction & inStreamFunction,
                              std::ostream & ostr)
    {
        ostr << "<" << inHTMLElement << ">";
        inStreamFunction(ostr);
        ostr << "</" << inHTMLElement << ">";
        return ostr;
    }

    
    std::string URIEncode(const std::string & inRawValue)
    {
        std::string result;
        Poco::URI::encode(inRawValue, " &=", result);
        return result;
    }


    std::string URIDecode(const std::string & inEncodedValue)
    {
        std::string result;
        Poco::URI::decode(inEncodedValue, result);
        return result;
    }

} // namespace HSServer
