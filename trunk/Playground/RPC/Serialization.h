#ifndef SERIALIZATION_H
#define SERIALIZATION_H


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <sstream>


typedef boost::tuples::tuple<std::string, std::string> NameAndArg;
typedef boost::tuples::tuple<bool, std::string> RetOrError;


/**
 * Generic serialize method
 */
template<typename T>
std::string serialize(const T & value)
{
    std::ostringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << value;
    return ss.str();
}


/**
 * Generic deserialize method
 */
template<typename T>
T deserialize(const std::string & buffer)
{
    std::istringstream ss(buffer);
    boost::archive::text_iarchive ia(ss);
    T ret;
    ia >> ret;
    return ret;
}


/**
 * Can be used to emulate void return value: `return Void();`
 */
struct Void
{
    template<class Archive>
    void serialize(Archive & , const unsigned int) { }
};


/**
 * Generic serialize for tuple types.
 * Feel free to extend the list.
 */
namespace boost {
namespace serialization {


template<typename Archive, typename T0>
void serialize(Archive & ar, tuple<T0> & t, const unsigned int)
{
    ar & get<0>(t);
}


template<typename Archive, typename T0, typename T1>
void serialize(Archive & ar, tuple<T0, T1> & t, const unsigned int)
{
    ar & get<0>(t);
    ar & get<1>(t);
}


template<typename Archive, typename T0, typename T1, typename T2>
void serialize(Archive & ar, tuple<T0, T1, T2> & t, const unsigned int)
{
    ar & get<0>(t);
    ar & get<1>(t);
    ar & get<2>(t);
}


template<typename Archive, typename T0, typename T1, typename T2, typename T3, typename T4>
void serialize(Archive & ar, tuple<T0, T1, T2, T3, T4> & t, const unsigned int)
{
    ar & get<0>(t);
    ar & get<1>(t);
    ar & get<2>(t);
    ar & get<3>(t);
    ar & get<4>(t);
}


template<typename Archive, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
void serialize(Archive & ar, tuple<T0, T1, T2, T3, T4, T5> & t, const unsigned int)
{
    ar & get<0>(t);
    ar & get<1>(t);
    ar & get<2>(t);
    ar & get<3>(t);
    ar & get<4>(t);
    ar & get<5>(t);
}


} } // namespace boost::serialization



#endif // SERIALIZATION_H
