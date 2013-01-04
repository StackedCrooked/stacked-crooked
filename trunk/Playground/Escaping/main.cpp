#include <boost/algorithm/string/replace.hpp>
#include <fstream>
#include <iostream>


static const std::string & file_data()
{
    static std::string fdata = [] {
        std::string s;
        std::ifstream is("input.txt");
        return std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
    }();
    return fdata;
}


typedef std::string delimiter;
typedef std::string escape;


void encode(std::string & s, delimiter delim, escape esc)
{
    if (s.find(esc + delim) != std::string::npos)
    {
        encode(s, esc + delim, esc);
    }
    boost::algorithm::replace_all(s, delim, esc + delim);
}



void decode(std::string & s, delimiter delim, escape esc)
{
    if (s.find(esc + delim) != std::string::npos)
    {
        boost::algorithm::replace_all(s, esc + delim, delim);
        decode(s, esc + delim, esc);
    }
}


std::string encode_copy(std::string s, delimiter delim, escape esc)
{
    encode(s, delim, esc);
    return s;
}


std::string decode_copy(std::string s, delimiter delim, escape esc)
{
    decode(s, delim, esc);
    return s;
}


int main()
{   
    std::string s = file_data(); 
    std::cout << s << std::endl << std::endl;
    
    std::string enc = encode_copy(s, "\t", "\\");
    std::cout << enc << std::endl << std::endl;
    std::ofstream("encoded.txt") << enc;
    
    std::string dec = decode_copy(enc, "\t", "\\");
    std::cout << dec << std::endl << std::endl;
    std::ofstream("decoded.txt") << dec;
}
