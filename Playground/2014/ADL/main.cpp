#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <typeinfo>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <cxxabi.h>


#define PTRACE  std::cout << __LINE__ << ':' << GetTypeName(self()) << "::" << __FUNCTION__ << std::endl;
#define CTRACE  std::cout << __PRETTY_FUNCTION__ << std::endl;
#define FTRACE  std::cout << __LINE__ << ':' << __FUNCTION__ << std::endl;


namespace Stack {


struct DataDecoder;



template<typename T>
T& GetProtocol()
{
    CTRACE
    static T t;
    return t;
}


std::string Demangle(const char * name)
{
    int st;
    std::shared_ptr<char> p(abi::__cxa_demangle(name, 0, 0, &st), std::free);
    if (st != 0)
    {
        switch (st)
        {
            case -1: throw std::runtime_error("A memory allocation failure occurred.");
            case -2: throw std::runtime_error("Not a valid name under the GCC C++ ABI mangling rules.");
            case -3: throw std::runtime_error("One of the arguments is invalid.");
            default: throw std::runtime_error("Unexpected demangle status");
        }
    }
    return std::string(p.get());
}

template<typename T>
static std::string& GetTypeName(T&& = {})
{
    static std::string fCached = []{
        auto s = Demangle(typeid(T).name());
        boost::algorithm::replace_all(s, "Stack::", "");
        boost::algorithm::replace_all(s, "IPv4::", "");
        return s;
    }();
    return fCached;
}


struct Protocol
{
    typedef void(*PopFunction)(Protocol&, const DataDecoder&);

    Protocol() {}

    Protocol(PopFunction inPopFunction) :
        mPopFunction(inPopFunction)
    {

    }

    void pop(const DataDecoder& str)
    {
        mPopFunction(*this, str);
    }

private:
    PopFunction mPopFunction;
};


struct DataDecoder
{
    DataDecoder(const DataDecoder&) = default;
    DataDecoder(const char* b, const char* e) : b(b), e(e) {}

    const char* data() const { return b; }
    std::size_t size() const { return e - b; }
    bool empty() const { return !size(); }

    const char* begin() const { return data(); }
    const char* end() const { return begin() + size(); }

private:
    const char* b;
    const char* e;
};


template<typename T, typename Header>
struct Decoder : DataDecoder
{
    Decoder(const DataDecoder& dd) : DataDecoder(dd) {}

    auto getPayloadData() const
    {
        return data() + getHeaderLength();
    }

    auto getHeaderLength() const
    {
        return self().getHeaderLengthImpl();
    }

    const char* data() const { return b; }
    std::size_t size() const { return e - b; }
    bool empty() const { return !size(); }

    const char* begin() const { return data(); }
    const char* end() const { return begin() + size(); }

private:
    const T& self() const { return static_cast<const T&>(*this); }
    T& self() { return static_cast<T&>(*this); }

    auto getHeaderLengthImpl() const
    {
        return sizeof(Header);
    }

    const char* b;
    const char* e;
};




template<typename CLP, typename DecoderType>
struct BasicProtocol : Protocol
{
private:
    typedef BasicProtocol<CLP, DecoderType> this_type;

public:
    BasicProtocol() :
        Protocol(&BasicProtocol::dynamic_pop)
    {
    }

    void pop(const DataDecoder& s)
    {
        PTRACE
        DecoderType dec(s);
        notify_pop(dec);
        self().do_pop(dec);
    }

protected:
    const CLP& self() const { return static_cast<const CLP&>(*this); }
    CLP& self() { return static_cast<CLP&>(*this); }

private:
    void notify_pop(const DecoderType& dec)
    {
        for (auto&& listener : mPopListeners)
        {
            listener(dec);
        }
    }

    friend std::string to_string(const this_type&)
    {
        std::string s = GetTypeName<CLP>();
        boost::algorithm::replace_first(s, "Stack::", "");
        boost::algorithm::replace_first(s, "IPv4::", "");
        return s;
    }

    template<typename ...T>
    friend void before_pop(BasicProtocol<T&...>&, const DecoderType&)
    {
        std::cout << __LINE__ << ':' << __FUNCTION__ << std::endl;
    }

    template<typename ...T>
    friend void after_pop(BasicProtocol<T...>&, const DecoderType&)
    {
        std::cout << __LINE__ << ':' << __FUNCTION__ << std::endl;
    }

    template<typename ...T>
    friend void actual_pop(BasicProtocol<T...>& clp, const DecoderType& dec)
    {
        std::cout << __LINE__ << ':' << __FUNCTION__ << std::endl;
        clp.default_pop(dec);
    }

    static void dynamic_pop(Protocol& p, const DataDecoder& s)
    {
        static_cast<CLP&>(p).pop(s);
    }

    std::vector<std::function<void(const DecoderType&)>> mPopListeners;
};


struct Session
{
    typedef void(*PushFunction)(Protocol&, const DataDecoder&);

    Session(Protocol& inProtocol, const PushFunction& /*inPushFunction*/) :
        mProtocol(inProtocol)//, mPushFunction(inPushFunction)
    {
    }

    Protocol& getProtocolBase()
    {
        return mProtocol;
    }

private:
    Protocol& mProtocol;
    //PushFunction mPushFunction;
};


template<typename CLP, typename CLS, typename Key>
struct BasicSession : Session
{
private:
    typedef BasicSession<CLP, CLS, Key> this_type;

public:
    BasicSession(CLP& inCLP, Key inKey) :
        Session(inCLP, &BasicSession::dynamic_pop),
        mKey(inKey)
    {
    }

    template<typename DecoderType>
    void pop(const DecoderType& dec)
    {
        PTRACE
        FindProtocol(dec).pop(dec);

    }

    const CLP& getProtocol() const { return static_cast<CLP&>(getProtocolBase()); }
    CLP& getProtocol() { return static_cast<CLP&>(getProtocolBase()); }

private:
    const CLS& self() const { return static_cast<const CLS&>(*this); }
    CLS& self() { return static_cast<CLS&>(*this); }

    static void dynamic_pop(Protocol& p, const DataDecoder& s)
    {
        static_cast<CLP&>(p).pop(s);
    }

    Key mKey;
};


template<typename CLP, typename CLS, typename SessionKey>
struct BasicSessionManager
{
    template<typename DecoderType>
    void do_pop(const DecoderType& dec)
    {
        this->sessionPop(dec);
    }

    template<typename DecoderType>
    void sessionPop(const DecoderType& dec)
    {
        auto key = GetSessionKey(dec);
        auto it = mSessions.find(key);
        if (it != mSessions.end())
        {
            it->second->pop(DataDecoder(dec.begin(), dec.end()));
            return;
        }

        auto cls = static_cast<CLP&>(*this).createSession(dec);
        mSessions.insert(std::make_pair(key, cls));
        cls->pop(dec);
    }

private:
    std::map<SessionKey, std::shared_ptr<CLS>> mSessions;
};


inline namespace IPv4 {


struct IPv4Address
{
    friend bool operator<(const IPv4Address& lhs, const IPv4Address& rhs)
    {
        return lhs.mBytes < rhs.mBytes;
    }

    friend bool operator==(const IPv4Address& lhs, const IPv4Address& rhs)
    {
        return lhs.mBytes == rhs.mBytes;
    }

    std::array<uint8_t, 4> mBytes;
};


typedef IPv4Address LocalIP;
typedef IPv4Address RemoteIP;


enum class IPProtNum : uint8_t
{
    ICMP,
    UDP,
    TCP
};

struct IPv4Header
{
    LocalIP mLocalIP;
    RemoteIP mRemoteIP;
    IPProtNum mIPProtNum;
};


struct IPv4Decoder : Decoder<IPv4Decoder, IPv4Header>
{
    IPv4Decoder(const DataDecoder& dd) : Decoder(dd) {}

    IPv4Address getSourceIP() const { return IPv4Address(); }
    IPv4Address getDestinationIP() const { return IPv4Address(); }
    IPProtNum getProtocol() const { return IPProtNum(); }
};

struct IPv4SessionKey
{
    IPv4SessionKey(const RemoteIP& inRemoteIP, const LocalIP& inLocalIP, const IPProtNum inProtocol) :
        mData(inRemoteIP, inLocalIP, inProtocol)
    {
    }

    friend bool operator<(const IPv4SessionKey& lhs, const IPv4SessionKey& rhs)
    {
        return lhs.mData < rhs.mData;
    }

private:
    std::tuple<RemoteIP, LocalIP, IPProtNum> mData;
};


inline IPv4SessionKey GetSessionKey(const IPv4Decoder& dec)
{
    return { dec.getSourceIP(), dec.getDestinationIP(), dec.getProtocol() };
}

struct IPv4Protocol;


struct IPv4Session : BasicSession<IPv4Protocol, IPv4Session, IPv4SessionKey>
{
    IPv4Session(IPv4Protocol&, IPv4SessionKey);

};


struct IPv4Protocol : BasicProtocol<IPv4Protocol, IPv4Decoder>, BasicSessionManager<IPv4Protocol, IPv4Session, IPv4SessionKey>
{
    std::shared_ptr<IPv4Session> createSession(const IPv4Decoder& dec)
    {
        return std::shared_ptr<IPv4Session>(new IPv4Session(*this, GetSessionKey(dec)));
    }


    void do_pop(const IPv4Decoder& dec)
    {
        PTRACE
        this->sessionPop(dec);
    }
};

IPv4Session::IPv4Session(IPv4Protocol& inIPv4Protocol, IPv4SessionKey inSessionKey) :
    BasicSession(inIPv4Protocol, inSessionKey)
{
}

//IPv4Session::IPv4Session(IPv4Protocol& ip, const IPv4Decoder& dec) :
//    BasicSession<IPv4Protocol, IPv4Session>(ip, dec),
//    mSessionKey(dec.getSourceIP(), dec.getDestinationIP(), dec.getProtocol())
//{
//    IPv4Protocol& getProtocol() { return static_cast<IPv4Protocol&>(this->getProtocolBase()); }
//}

//template<> struct Decoder2Protocol<struct EthernetDecoder> { typedef struct EthernetProtocol type; };
//template<> struct Decoder2Protocol<IPv4Decoder> { typedef IPv4Protocol type; };
//template<> struct Decoder2Protocol<struct ICMPDecoder> { typedef struct ICMPProtocol type; };

} // IPv4


struct ICMPProtocol : Protocol {};
struct TCPProtocol : Protocol {};
struct UDPProtocol : Protocol {};


Protocol& FindProtocol(const IPv4Decoder& dec)
{
    typedef std::function<Protocol&()> Getter;
    std::vector<std::pair<IPProtNum, Getter>> items = {
        { IPProtNum::ICMP, GetProtocol<ICMPProtocol> },
        { IPProtNum::TCP , GetProtocol<TCPProtocol > },
        { IPProtNum::UDP , GetProtocol<UDPProtocol > }
    };

    return items.at(static_cast<uint8_t>(dec.getProtocol())).second();
}


} // Stack



int main()
{
    using namespace Stack;

    std::string s = "abc";
    IPv4Protocol ipv4Protocol;
    ipv4Protocol.pop(DataDecoder{ s.data(), s.data() + s.size() });
}


namespace Stack { inline namespace IPv4 {


void before_pop(const IPv4Protocol&, const IPv4Decoder&)
{
    FTRACE
}


void after_pop(const IPv4Protocol&, const IPv4Decoder&)
{
    FTRACE
}


void replace_pop(const IPv4Protocol&, const IPv4Decoder&)
{
    FTRACE
}


} }

