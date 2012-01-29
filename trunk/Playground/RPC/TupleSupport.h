#ifndef TUPLESUPPORT_H
#define TUPLESUPPORT_H

#include <boost/serialization/serialization.hpp>
#include <boost/tuple/tuple.hpp>


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


#endif // TUPLESUPPORT_H
