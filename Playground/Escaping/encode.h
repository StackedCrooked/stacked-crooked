#include <boost/algorithm/string/replace.hpp>


typedef std::string delimiter;
typedef std::string escape;


inline void encode(std::string & s, delimiter delim, escape esc)
{
    boost::algorithm::replace_all(s, esc, esc + esc);
    boost::algorithm::replace_all(s, delim, esc + delim);
}



inline void decode(std::string & s, delimiter delim, escape esc)
{
    boost::algorithm::replace_all(s, esc + delim, delim);
    boost::algorithm::replace_all(s, esc + esc, esc);
}


inline std::string encode_copy(std::string s, delimiter delim, escape esc)
{
    encode(s, delim, esc);
    return s;
}


inline std::string decode_copy(std::string s, delimiter delim, escape esc)
{
    decode(s, delim, esc);
    return s;
}
