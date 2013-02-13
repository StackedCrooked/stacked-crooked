#include <array>
#include <iostream>
#include <stdint.h>


template<typename T>
struct InvalidEnumerator : std::runtime_error
{
    InvalidEnumerator(T t) : std::runtime_error("InvalidEnumerator") {}
    ~InvalidEnumerator() throw() {}
    
    T getEnumerator() const
    {
        return t;
    }
    
    T t;
};

template<typename T>
InvalidEnumerator<T> InvalidEnumerator(T t)
{
    return InvalidEnumerator<T>(t);
}

template<typename T, typename /*disambiguator*/>
struct StrongTypedef
{
    StrongTypedef(const T data = T()) : data_(data) {}
    operator const T & () const
    {
        return data_;
    }
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
    T hostValue()
    {
        return Net2Host(_value);
    }
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

using Raw = std::pair<uint8_t *, uint8_t *>;

Raw Inc(Raw raw, unsigned n)
{
    assert(raw.first + n <= raw.second);
    return Raw(raw.first + n, raw.second);
}

template<typename Header>
Raw Inc(Raw raw)
{
    return Raw(raw.first + sizeof(Header), raw.second);
}

EtherType GetHLPId(EthernetFrame eth)
{
    return static_cast<EtherType>(eth.mEtherType.hostValue());
}

EtherType GetHLPId(VLANTag vlan)
{
    return static_cast<EtherType>(vlan.mEtherType.hostValue());
}

IPProtNum GetHLPId(IPPacket ip)
{
    return static_cast<IPProtNum>(ip.mProtocol);
}

template<typename Tail>
auto GetHLPId(std::pair<Raw, Tail> msg) -> decltype(GetHLPId(msg.second))
{
    return GetHLPId(msg.second);
}

void pop(const Raw & inRaw);
void pop(std::pair<Raw, EthernetFrame> data);
template<typename Tail> void pop(std::pair<Raw, std::pair<IPPacket     , Tail> > data);
template<typename Tail> void pop(std::pair<Raw, std::pair<ICMPMessage  , Tail> > data);

template<typename Head>
std::pair<Raw, Head> Decode(Raw raw)
{
    return std::make_pair(Inc<Head>(raw), Decode<Head>(raw));
}

template<typename Head, typename Tail>
std::pair<Raw, std::pair<Head, Tail> > Decode(const std::pair<Raw, Tail> & msg)
{
    return std::make_pair(Inc<Head>(msg.first), std::make_pair(Decode<Head>(msg.first), msg.second));
}

void pop(Raw raw)
{
    return pop(Decode<EthernetFrame>(raw));
}

template<typename Header, typename Tail>
auto GetHLPId(std::pair<Header, Tail> data) -> decltype(GetHLPId(data.second))
{
    return GetHLPId(data.first);
}

void pop(std::pair<Raw, EthernetFrame> data)
{
    switch (GetHLPId(data))
    {
        case EtherType::ARP:
        {
            pop(Decode<ARPMessage>(data));
            break;
        }
        case EtherType::IP :
        {
            pop(Decode<IPPacket>(data));
            break;
        }
        default:
        {
            throw InvalidEnumerator(data.first.mEtherType);
        }
    }
}

template<typename Tail>
void pop(std::pair<Raw, std::pair<IPProtNum, Tail> > msg)
{
    switch (GetHLPId(msg))
    {
        case IPProtNum::ICMP:
        {
            pop(Decode<ICMPMessage>(msg));
            break;
        }
        default:
        {
             throw InvalidEnumerator(protocol);
        }
    }
}

template<typename T> Flip(T) {}

template<typename Tail>
void pop(std::pair<Raw, std::pair<ICMPMessage, Tail> > msg)
{
    Packet packet;
    packet.push(Flip(msg.first));
    push(packet, msg.second);
}

