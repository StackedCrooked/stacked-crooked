#include <array>
#include <iostream>
#include <stdint.h>

template<typename T, typename /*disambiguator*/>
struct StrongTypedef {
    StrongTypedef(const T data = T()) : data_(data) {}
    operator const T&() const { return data_; }
    T data_;
};

#define STRONG_TYPEDEF(Type, Name) typedef StrongTypedef<Type, struct Name##_> Name;


uint16_t Net2Host(uint16_t v) { return ntohs(v); }
uint32_t Net2Host(uint32_t v) { return ntohl(v); }

template<typename T>
struct NetEncoded
{
    T hostValue() { return Net2Host(_value); }
    T _value;
};


typedef NetEncoded<uint16_t> Net16;
typedef NetEncoded<uint32_t> Net32;

typedef std::array<uint8_t, 6> MAC;
typedef MAC SourceMAC;
typedef MAC TargetMAC;
typedef MAC LocalMAC;
typedef MAC RemoteMAC;

enum class EtherType : uint16_t
{
    ARP   = 0x0806,
    IP  = 0x0800,
    IP6  = 0x86DD
};

struct EthernetFrame
{
    Net16 mEtherType;
    TargetMAC mTarget;
    SourceMAC mSource;
};


struct VLANTag
{
    Net16 mEtherType;
    uint16_t mData;
};


typedef std::array<uint8_t, 4> IP;
typedef IP SourceIP;
typedef IP TargetIP;
typedef IP LocalIP;
typedef IP RemoteIP;

enum class IPProtNum : uint8_t
{
    ICMP = 1,
    TCP  = 6,
    UDP  = 17
};

struct IPPacket
{
    uint8_t   mVersionAndIHL; // -1
    uint8_t   mTypeOfService; // 1
    Net16     mTotalLength;   // 2
    Net16     mIdentification; // 4
    Net16     mFlagsAndFragmentOffset; // 6
    uint8_t   mTTL; // 8
    IPProtNum mProtocol; // 9
    Net16     mHeaderChecksum; // 10
    SourceIP  mSourceIP; // 12
    TargetIP  mTargetIP; // 16
};


enum class ARPOperation : uint16_t
{
    Request = 1,
    Response = 2
};

struct ARPMessage
{
    Net16 HTYPE;
    Net16 PTYPE;
    uint8_t HLEN;
    uint8_t PLEN;
    Net16 Operation;

    SourceMAC SHA;
    SourceIP  SPA;
    TargetMAC THA;
    TargetIP  TPA;
};

enum class ICMPType : uint8_t
{
    EchoReply   = 0,
    EchoRequest = 8
};

struct ICMPMessage
{
    ICMPType mType;
    uint8_t  mCode;
    Net16 mCheckSum;
    Net16 mIdentifier;
    Net16 mSequenceNumber;
};

using Raw = std::pair<uint8_t*, uint8_t*>;
Raw increment(Raw raw, unsigned n)
{
    assert(raw.first + n <= raw.second);
    return Raw(raw.first + n, raw.second);
}

template<typename Header, typename Tail>
using Message = std::pair<Raw, std::pair<Header, Tail> >;


template<typename Header>
Raw RemoveHeader(Raw raw)
{
    return Raw(raw.first + sizeof(Header), raw.second);
}

template<typename Head>
Message<Head, nullptr_t> Decode(Raw raw)
{
    return Message(Raw(raw.first + sizeof(Head)), *reinterpret_cast<Head*>(raw.first), nullptr);
}

template<typename T>
struct InvalidEnumerator : std::runtime_error
{
    InvalidEnumerator(T t) : std::runtime_error("InvalidEnumerator") {}
    ~InvalidEnumerator() throw() {}

    T getEnumerator() const { return t; }

    T t;
};

template<typename T>
void ThrowInvalidEnumerator(T t)
{
    throw InvalidEnumerator<T>(t);
}

EtherType GetHLPId(VLANTag vlan)      { return static_cast<EtherType>(vlan.mEtherType.hostValue()); }
EtherType GetHLPId(EthernetFrame eth) { return static_cast<EtherType>(eth.mEtherType.hostValue()); }
IPProtNum GetHLPId(IPPacket ip)       { return static_cast<IPProtNum>(ip.mProtocol); }

template<typename Tail>
auto GetHLPId(std::pair<Raw, Tail> tail) -> decltype(GetHLPId(tail.second))
{
    return GetHLPId(tail.second);
}

void pop(const Raw & inRaw);

void pop(std::pair<Raw, EthernetFrame> data);
template<typename Tail> void pop(std::pair<Raw, std::pair<IPPacket     , Tail> > data);
template<typename Tail> void pop(std::pair<Raw, std::pair<ICMPMessage  , Tail> > data);

void pop(Raw raw)
{
    return pop(Decode<EthernetFrame>(raw));
}


template<typename Header> 
auto GetHLPId(std::pair<Raw, Header> data) -> decltype(GetHLPId(data.second))
{ return GetHLPId(data.second); }


template<typename Header, typename Tail>
auto GetHLPId(std::pair<Header, Tail> data) -> decltype(GetHLPId(data.second))
{ return GetHLPId(data.first); }


void pop(std::pair<Raw, EthernetFrame> data)
{
    switch (GetHLPId(data))
    {
        case EtherType::ARP: { pop(Decode<ARPMessage>(data)); break; }
        case EtherType::IP : { pop(Decode<IPPacket  >(data)); break; }
        default: ThrowInvalidEnumerator(data.first.mEtherType);
    }
}

template<typename Tail>
void pop(Message<IPProtNum, Tail> data)
{
    switch (GetHLPId(data))
    {
        case IPProtNum::ICMP: { pop(Decode<ICMPMessage>(data)); break; }
        default: ThrowInvalidEnumerator(protocol);
    }
}

template<typename Tail>
void pop(Message<ICMPMessage, Tail> data)
{
    Packet packet;
    packet.push(Flip(data.first));
    push(packet, Chop(data));
}

