#ifndef FILEUTILS_H_INCLUDED
#define FILEUTILS_H_INCLUDED


#include <string>


namespace HSServer
{

    void ReadEntireFile(const std::string & inPath, std::string & str);

}


#endif // FILEUTILS_H_INCLUDED
