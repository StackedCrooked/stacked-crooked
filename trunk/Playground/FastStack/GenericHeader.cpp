#ifndef GENERIC_HEADER_H
#define GENERIC_HEADER_H


#include <array>
#include <iostream>
#include <iomanip>
#include <utility>
#include <stdint.h>
#include <sstream>
#include <arpa/inet.h>
#include <boost/preprocessor.hpp>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__);
#define STATIC_ASSERT_IS_POD(T) STATIC_ASSERT(std::is_pod<T>::value)

#define HEADER_MEMBER(A, B) \
    A.B

#define HEADER_PRINT_FIELD(r, Type, Elem) \
    os << "  " << BOOST_PP_STRINGIZE(Elem) << ": " << HEADER_MEMBER(Type, BOOST_PP_CAT(m, Elem)) << "\n";

#define HEADER_IMPL(r, _, Elem) \
    Elem BOOST_PP_CAT(m, Elem);

#define HEADER_TUPLE_ELEMENT(r, Type, Elem) \
    Type

#define HEADER_TUPLE(Name, Fields) \
    struct Tuple<Name> { \
        using type = std::tuple< BOOST_PP_SEQ_FOR_EACH(HEADER_IMPL, _, Fields) >; \
    };

#define HEADER(Name, Fields) struct Name { \
        BOOST_PP_SEQ_FOR_EACH(HEADER_IMPL, _, Fields) \
    }; \
    std::ostream& operator<<(std::ostream& os, const Name & BOOST_PP_CAT(in, Name)) { \
        os << BOOST_PP_STRINGIZE(Name) << ": " << std::endl; \
        BOOST_PP_SEQ_FOR_EACH(HEADER_PRINT_FIELD, BOOST_PP_CAT(in, Name), Fields) \
        return os << std::flush; \
    }


// Type-wrapper that generate defines a "strong" typedef while retaining POD-ness.
template<typename T, typename /*disambiguator*/>
struct Wrap
{
    T & get() { return obj; }
    const T & get() const { return obj; }

    operator T() const { return obj; }

    T obj;

    STATIC_ASSERT(std::is_pod<T>::value)
};


using MACField  = Wrap<uint8_t, struct MACField_>;
using MACArray  = std::array<MACField, 6>;
using SourceMAC = Wrap<MACArray, struct SourceMAC_>;
using TargetMAC = Wrap<MACArray, struct TargetMAC_>;


STATIC_ASSERT_IS_POD(MACField)
STATIC_ASSERT_IS_POD(MACArray)
STATIC_ASSERT_IS_POD(SourceMAC)
STATIC_ASSERT_IS_POD(TargetMAC)


using IPField  = Wrap<uint8_t, struct IPField_>;
using IPArray  = std::array<IPField, 4>;
using SourceIP = Wrap<IPArray, struct SourceIP_>;
using TargetIP = Wrap<IPArray, struct TargetIP_>;


STATIC_ASSERT_IS_POD(IPField)
STATIC_ASSERT_IS_POD(IPArray)
STATIC_ASSERT_IS_POD(SourceIP)
STATIC_ASSERT_IS_POD(TargetIP)


enum class EtherType : uint16_t
{
    ARP   = 0x0806,
    IP    = 0x0800,
    IP6   = 0x86DD
};


std::ostream& operator<<(std::ostream & os, const MACField & inByte)
{
    return os << std::setw(2) << std::setfill('0') << static_cast<int>(inByte);
}


std::ostream& operator<<(std::ostream & os, const IPField & inByte)
{
    return os << static_cast<int>(inByte);
}



std::ostream& operator<<(std::ostream & os, const MACArray & inMAC)
{
    return os << inMAC[0] << ":" << inMAC[1] << ":" << inMAC[2] << ":" << inMAC[3] << ":" << inMAC[4] << ":" << inMAC[5];
}



std::ostream& operator<<(std::ostream & os, const IPArray & inIP)
{
    return os << inIP[0] << "." << inIP[1] << "." << inIP[2] << "." << inIP[3];
}



std::ostream& operator<<(std::ostream& os, const EtherType & inEtherType)
{
    switch (inEtherType)
    {
        case EtherType::ARP: return os << "ARP";
        case EtherType::IP : return os << "IP";
        case EtherType::IP6: return os << "IP6";
        default: return os << "Invalid";
    };
}




typedef uint16_t IPVersion;

HEADER(EthernetFrame,
       (TargetMAC)
       (SourceMAC)
       (EtherType))

HEADER(IPPacket,
       (IPVersion)
       (SourceIP)
       (TargetIP))


STATIC_ASSERT(std::is_pod<EthernetFrame>::value)
STATIC_ASSERT(std::is_pod<IPPacket>::value)

int main()
{
    TargetMAC dst = { 0, 0, 0, 0, 0, 1 };
    SourceMAC src = { 0, 0, 0, 0, 0, 2 };
    EthernetFrame eth = { dst, src, EtherType::ARP };
    std::swap(eth.mSourceMAC.get(), eth.mTargetMAC.get());
    std::cout << eth << std::endl;

    IPPacket ip = {
        4,
        { 10, 0, 0, 1 },
        { 10, 0, 0, 2 }
    };
    std::cout << ip << std::endl;
}


#endif // GENERIC_HEADER_H
