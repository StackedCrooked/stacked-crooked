#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED


#include <boost/function.hpp>
#include <map>
#include <string>


namespace HSServer
{

    /**
     * Reads the entire contents of a text-encoded file in a string.
     */
    void ReadEntireFile(const std::string & inPath, std::string & str);


    /**
     * Args datatype.
     */
    typedef std::map<std::string, std::string> Args;


    /**
     * Efficiently parses the query arguments of an URI.
     */
    void GetArgs(const std::string & inURI, Args & outArgs);

    /**
     * Gets an argument by name.
     * Throws MissingArgumentException if not found.
     * Throws  if not found.
     */
    const std::string & GetArg(const Args & inArgs, const std::string & inArg);

    enum HTMLFormatting
    {
        HTMLFormatting_NoBreaks,
        HTMLFormatting_OneLiner,
        HTMLFormatting_ThreeLiner
    };

    /**
     * Wraps text in a HTML tag.
     * For example `MakeHTML("p", "Hello!");` will return "<p>Hello!</p>"
     */
    std::string MakeHTML(const std::string & inHTMLElement, const std::string & inText, HTMLFormatting inHTMLFormatting);

    /**
     * Stream-based version of MakeHTML.
     */
    std::ostream & StreamHTML(const std::string & inHTMLElement,
                              const std::string & inText,
                              HTMLFormatting inHTMLFormatting,
                              std::ostream & ostr);


    typedef boost::function<void(std::ostream &)> StreamFunction;

    /**
     * Stream-based verion of MakeHTML using a callback.
     */      
    std::ostream & StreamHTML(const std::string & inHTMLElement,
                              const StreamFunction & inStreamFunction,
                              HTMLFormatting inHTMLFormatting,
                              std::ostream & ostr);


    std::string URIEncode(const std::string & inRawValue);
    std::string URIDecode(const std::string & inEncodedValue);

}


#endif // UTILS_H_INCLUDED
