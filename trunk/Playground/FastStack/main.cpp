#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
#include <iostream>
#include <vector>
#include <stdint.h>


typedef std::vector<uint8_t> DynamicBuffer;
typedef std::vector<DynamicBuffer> DynamicBuffers;


class Packet
{
public:
    Packet() = default; // enable default constructor.

    template<typename T>
    explicit Packet(const T & inInitialData)
    {
        push_front(inInitialData);
    }

    explicit Packet(DynamicBuffer inData)
    {
        push_front(std::move(inData));
    }

    // Move constructor.
    Packet(Packet&& inPacket) : mParts(std::move(inPacket.mParts)) { }

    Packet & operator=(Packet && inPacket)
    {
        mParts = std::move(inPacket.mParts);
        return *this;
    }

    void push_front(const uint8_t * inData, std::size_t inSize)
    {
        mParts.push_front(DynamicBuffer(inData, inData + inSize));
    }

    void push_front(const DynamicBuffer & inData)
    {
        mParts.push_front(inData);
    }

    void push_back(const DynamicBuffer & inData)
    {
        mParts.push_back(inData);
    }

    template<typename T>
    void push_back(const T & inHeader)
    {
        mParts.push_back(std::move(ConvertHeaderToString(inHeader)));
    }

    std::size_t getTotalSize() const
    {
        auto op = [](std::size_t sum, const DynamicBuffer & str) -> std::size_t {
            return sum + str.size();
        };
        return std::accumulate(mParts.begin(), mParts.end(), 0u, op);
    }

    void get(DynamicBuffer & outData) const
    {
        outData.clear();
        outData.reserve(getTotalSize());

        for (const auto & str : mParts)
        {
            outData.insert(outData.end(), str.begin(), str.end());
        }
    }

    DynamicBuffer getBuffer() const
    {
        DynamicBuffer result;
        get(result);
        return result;
    }

private:
    template<typename T>
    static DynamicBuffer ConvertHeaderToString(const T & inHeader)
    {
        static_assert(std::is_pod<T>::value, "Header type must be POD type!");
        const uint8_t * data = reinterpret_cast<const uint8_t*>(&inHeader);
        return DynamicBuffer(data, data + sizeof(inHeader));
    }

    Packet(const Packet&) = delete; // disable copy
    Packet& operator=(const Packet&) = delete; // disable assigment

    std::deque<DynamicBuffer> mParts;
};


inline DynamicBuffer GetPacketData(const Packet & inPacket)
{
    DynamicBuffer result;
    inPacket.get(result);
    return result;
}



template<typename T>
struct InvalidEnumerator : std::runtime_error
{
    InvalidEnumerator(T t) : std::runtime_error("InvalidEnumerator"), t(t) {}
    ~InvalidEnumerator() throw() {}
    
    T getEnumerator() const
    {
        return t;
    }
    
    T t;
};

template<typename T>
InvalidEnumerator<T> MakeInvalidEnumerator(T t)
{
    return InvalidEnumerator<T>(t);
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

struct Raw : std::pair<uint8_t*, uint8_t*>
{
    template<typename ...Args>
    Raw(Args && ...args) : std::pair<uint8_t*, uint8_t*>(std::forward<Args>(args)...) {}
};


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

void pop(const Raw & inRaw);
void pop(std::pair<Raw, EthernetFrame> msg);
template<typename Tail> void pop(std::pair<Raw, EthernetFrame> msg);
template<typename Tail> void pop(std::pair<Raw, std::pair<IPPacket     , Tail> > msg);
template<typename Tail> void pop(std::pair<Raw, std::pair<ARPMessage   , Tail> > msg);
template<typename Tail> void pop(std::pair<Raw, std::pair<ICMPMessage  , Tail> > msg);

template<typename Head>
std::pair<Raw, Head> Decode(Raw raw)
{
    return std::make_pair(Inc<Head>(raw), *reinterpret_cast<Head*>(raw.first));
}

template<typename Head, typename Tail>
std::pair<Raw, std::pair<Head, Tail> > Decode(const std::pair<Raw, Tail> & msg)
{
    auto p = Decode<Head>(msg.first);
    return std::make_pair(p.first, std::make_pair(p.second, msg.second));
}

void pop(Raw raw)
{
    return pop(Decode<EthernetFrame>(raw));
}

template<typename Header>
auto GetHLPId(std::pair<Raw, Header> msg) -> decltype(GetHLPId(msg.second))
{
    return GetHLPId(msg.second);
}

template<typename Header, typename Tail>
auto GetHLPId(std::pair<Header, Tail> msg) -> decltype(GetHLPId(msg.first))
{
    return GetHLPId(msg.first);
}

void pop(std::pair<Raw, EthernetFrame> msg)
{
    auto hlpId = GetHLPId(msg);
    switch (hlpId)
    {
        case EtherType::ARP:
        {
            pop(Decode<ARPMessage>(msg));
            break;
        }
        case EtherType::IP:
        {
            pop(Decode<IPPacket>(msg));
            break;
        }
        default:
        {
            throw MakeInvalidEnumerator(hlpId);
        }
    }
}

template<typename Tail>
void pop(std::pair<Raw, std::pair<IPPacket, Tail> > msg)
{
    auto hlpId = GetHLPId(msg);
    switch (hlpId)
    {
        case IPProtNum::ICMP:
        {
            pop(Decode<ICMPMessage>(msg));
            break;
        }
        default:
        {
             throw MakeInvalidEnumerator(hlpId);
        }
    }
}

template<typename T> T Flip(T t)
{
    return t;
}

template<typename Tail>
void pop(std::pair<Raw, std::pair<ICMPMessage, Tail> > msg)
{
    Packet packet;
    auto flipped = Flip(msg.second.first);
    static_assert(std::is_pod<decltype(flipped)>::value, "");
    uint8_t* bytes = reinterpret_cast<uint8_t*>(&flipped);
    packet.push_front(bytes, sizeof(flipped));
    //push(packet, msg.second);
}

template<typename Tail>
void pop(std::pair<Raw, std::pair<ARPMessage, Tail> > msg)
{
    Packet packet;
    Raw raw = msg.first;
    packet.push_front(raw.first, raw.second - raw.first);
    //push(msg.second);
}


int main()
{
}
