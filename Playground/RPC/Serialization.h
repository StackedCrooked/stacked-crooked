#ifndef SERIALIZATION_H
#define SERIALIZATION_H


#include "TupleSupport.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
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


struct Void
{
    template<class Archive>
    void serialize(Archive & , const unsigned int) { }
};


typedef boost::tuples::tuple<std::string, std::string> NameAndArg;
typedef boost::tuples::tuple<bool, std::string> RetOrError;


#endif // SERIALIZATION_H
