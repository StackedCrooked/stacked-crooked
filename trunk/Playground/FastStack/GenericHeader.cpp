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


#define HEADER_MEMBER(A, B) A.B
#define HEADER_PRINT_FIELD(r, Type, Elem) os << "  " << BOOST_PP_STRINGIZE(Elem) << ": " << HEADER_MEMBER(Type, BOOST_PP_CAT(m, Elem)) << "\n";
#define HEADER_IMPL(r, _, Elem) Elem BOOST_PP_CAT(m, Elem);


#define HEADER_TUPLE_ELEMENT(r, Type, Elem) Type
#define HEADER_TUPLE(Name, Fields) struct Tuple<Name> { using type = std::tuple< BOOST_PP_SEQ_FOR_EACH(HEADER_IMPL, _, Fields) >; };


#define HEADER(Name, Fields) struct Name { \
        BOOST_PP_SEQ_FOR_EACH(HEADER_IMPL, _, Fields) \
    }; \
    std::ostream& operator<<(std::ostream& os, const Name & BOOST_PP_CAT(in, Name)) { \
        os << BOOST_PP_STRINGIZE(Name) << ": " << std::endl; \
        BOOST_PP_SEQ_FOR_EACH(HEADER_PRINT_FIELD, BOOST_PP_CAT(in, Name), Fields) \
        return os << std::flush; \
    }



template<typename T, typename /*disambiguator*/>
struct StrongTypedef
{
    template<typename ...Args>
    StrongTypedef(Args && ...args) : data_(std::forward<Args>(args)...) {}
    operator const T & () const
    {
        return data_;
    }
    T & get() { return data_; }
    const T & get() const { return data_; }
    T data_;
};

#define STRONG_TYPEDEF(Type, Name) typedef StrongTypedef<Type, struct Name##_> Name;


uint16_t Net2Host(uint16_t v)
{
    return ntohs(v);
}
uint32_t Net2Host(uint32_t v)
{
    return ntohl(v);
}

template<typename T>
struct NetEncoded
{
    T hostValue() const
    {
        return Net2Host(_value);
    }
    T _value;
};


typedef NetEncoded<uint16_t> Net16;
typedef NetEncoded<uint32_t> Net32;

template<typename T, typename = int>
struct Wrap
{
    T & get() { return obj; }
    const T & get() const { return obj; }

    operator T() const { return obj; }

    T obj;
};

typedef Wrap<uint8_t, struct MACField_> MACField;

typedef std::array<MACField, 6> MAC;
typedef Wrap<MAC, struct SourceMAC_> SourceMAC;
typedef Wrap<MAC, struct TargetMAC_> TargetMAC;

typedef Wrap<Wrap<Wrap<MAC>>> DeepMAC;




typedef Wrap<uint8_t, struct IPField_> IPField;
typedef std::array<IPField, 4> IP;
typedef Wrap<IP, struct SourceIP_> SourceIP;
typedef Wrap<IP, struct DestinationIP_> DestinationIP;


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



std::ostream& operator<<(std::ostream & os, const MAC & inMAC)
{
    return os << inMAC[0] << ":" << inMAC[1] << ":" << inMAC[2] << ":" << inMAC[3] << ":" << inMAC[4] << ":" << inMAC[5];
}



std::ostream& operator<<(std::ostream & os, const IP & inIP)
{
    return os << inIP[0] << "." << inIP[1] << "." << inIP[2] << "." << inIP[3];
}



std::ostream& operator<<(std::ostream& os, const EtherType & inEtherType)
{
    switch (inEtherType)
    {
        case EtherType::ARP: return os << "ARP";
        case EtherType::IP: return os << "IP";
        case EtherType::IP6: return os << "IP6";
        default: return os << "Invalid";
    };
}

template<typename T, int N>
struct TuplePOD_
{
    T t;
};


STATIC_ASSERT(std::is_pod<Net16>::value)
STATIC_ASSERT(std::is_pod<Wrap<Net16>>::value)
STATIC_ASSERT(std::is_pod<SourceMAC>::value)
STATIC_ASSERT(std::is_pod<TargetMAC>::value)




typedef uint16_t IPVersion;

HEADER(EthernetFrame,
       (TargetMAC)
       (SourceMAC)
       (EtherType))

HEADER(IPPacket,
       (IPVersion)
       (SourceIP)
       (DestinationIP))


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
