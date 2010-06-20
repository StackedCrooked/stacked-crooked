#include "Exceptions.h"
#include "Utils.h"
#include "Poco/URI.h"
#include <algorithm>
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
    
    
    void MakeLowerCase(std::string & inText)
    {
        std::transform(inText.begin(), inText.end(), inText.begin(), ::tolower);
    }

    std::string MakeHTML(const std::string & inHTMLElement, const std::string & inText, HTMLFormatting inHTMLFormatting)
    {
        switch (inHTMLFormatting)
        {
            case HTMLFormatting_NoBreaks:
            {
                return "<" + inHTMLElement + ">" + inText + "</" + inHTMLElement + ">";
            }
            case HTMLFormatting_OneLiner:
            {
                return "<" + inHTMLElement + ">" + inText + "</" + inHTMLElement + ">\n";
            }
            case HTMLFormatting_ThreeLiner:
            {
                return "<" + inHTMLElement + ">\n" + inText + "\n</" + inHTMLElement + ">\n";
            }
            default:
            {
                throw std::logic_error("Invalid enum value.");
            }
        }
    }


    std::ostream & StreamHTML(const std::string & inHTMLElement, const std::string & inText, HTMLFormatting inHTMLFormatting, std::ostream & ostr)
    {
        switch (inHTMLFormatting)
        {
            case HTMLFormatting_NoBreaks:
            {
                return ostr << "<" << inHTMLElement << ">" << inText << "</" << inHTMLElement << ">";
            }
            case HTMLFormatting_OneLiner:
            {
                return ostr << "<" << inHTMLElement << ">" << inText << "</" << inHTMLElement << ">\n";
            }
            case HTMLFormatting_ThreeLiner:
            {
                return ostr << "<" << inHTMLElement << ">\n" << inText << "\n</" << inHTMLElement << ">\n";
            }
            default:
            {
                throw std::logic_error("Invalid enum value.");
            }
        }
    }
    
    
    std::ostream & StreamHTML(const std::string & inHTMLElement,
                              const StreamFunction & inStreamFunction,
                              HTMLFormatting inHTMLFormatting,
                              std::ostream & ostr)
    {
        switch (inHTMLFormatting)
        {
            case HTMLFormatting_NoBreaks:
            case HTMLFormatting_OneLiner:
            {
                ostr << "<" << inHTMLElement << ">";
                break;
            }
            case HTMLFormatting_ThreeLiner:
            {
                ostr << "<" << inHTMLElement << ">\n";
                break;
            }
            default:
            {
                throw std::logic_error("Invalid enum value.");
            }
        }
        inStreamFunction(ostr);
        switch (inHTMLFormatting)
        {
            case HTMLFormatting_NoBreaks:
            {
                ostr << "</" << inHTMLElement << ">";
                break;
            }
            case HTMLFormatting_OneLiner:
            case HTMLFormatting_ThreeLiner:
            {
                ostr << "</" << inHTMLElement << ">\n";
                break;
            }
            default:
            {
                throw std::logic_error("Invalid enum value.");
            }
        }
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
