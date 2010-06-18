#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED


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

}


#endif // UTILS_H_INCLUDED
