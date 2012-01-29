#ifndef SERIALIZATION_H
#define SERIALIZATION_H


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>
#include <sstream>


template<typename T>
std::string serialize(const T & value)
{
    std::ostringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << value;
    return ss.str();
}


template<typename T>
T deserialize(const std::string & buffer)
{
    std::istringstream ss(buffer);
    boost::archive::text_iarchive ia(ss);
    T ret;
    ia >> ret;
    return ret;
}


#endif // SERIALIZATION_H
