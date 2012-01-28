#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/tuple/tuple.hpp>
#include <sstream>


namespace RPC {


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


} // namespace RPC


namespace boost {
namespace serialization {


using namespace RPC;


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


template<typename Archive, typename T0, typename T1, typename T2, typename T3>
void serialize(Archive & ar, tuple<T0, T1, T2, T3> & t, const unsigned int)
{
    ar & get<0>(t);
    ar & get<1>(t);
    ar & get<2>(t);
    ar & get<3>(t);
}


} } // namespace boost::serialization
