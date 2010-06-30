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

    
    /**
     * Transform a string into its lower-case equivalent.
     */
    void MakeLowerCase(std::string & inText);


    /**
     * HTMLFormatting indicates how the html code is formatted.
     * 
     * - HTMLFormatting_NoBreaks    no new lines are created
     * - HTMLFormatting_OneLiner    one new line after the closing tag
     * - HTMLFormatting_ThreeLiner  add new line after opening and closing tag
     */
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

    
    /**
     * Stream-based verion of MakeHTML using a callback.
     */      
    typedef boost::function<void(std::ostream &)> StreamFunction;
    std::ostream & StreamHTML(const std::string & inHTMLElement,
                              const StreamFunction & inStreamFunction,
                              HTMLFormatting inHTMLFormatting,
                              std::ostream & ostr);


    static const char * cHTMLTemplate = 
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"                          "
        "                      \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">        "
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">                                             "
        "<head>                                                                                    "                                        
        "<title>{{title}}</title>                                                                  "
        "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />                 "
        "<meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />                           "
        "</head>                                                                                   "
        "<body>                                                                                    "
        "{{body}}                                                                                  "
        "</body>                                                                                   "
        "</html>                                                                                   ";


    std::string MakeHTMLDocument(const std::string & inTitle, const std::string & inBody);


    /**
     * URI encode/decode
     */
    std::string URIEncode(const std::string & inRawValue);
    std::string URIDecode(const std::string & inEncodedValue);

}


#endif // UTILS_H_INCLUDED
