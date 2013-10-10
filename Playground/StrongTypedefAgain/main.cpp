#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>

template<typename El, typename Tuple, int Index, int Size>
struct IndexOf_ { static constexpr int value = std::is_same<typename std::tuple_element<Index, Tuple>::type, El>::value ? Index : IndexOf_<El, Tuple, Index + 1, Size>::value; };

template<typename El, typename Tuple, int Index>
struct IndexOf_<El, Tuple, Index, Index> { static constexpr int value = -1; };

template<typename T, typename ...TupleTypes>
struct IndexOf;

template<typename T, typename ...TupleTypes>
struct IndexOf<T, std::tuple<TupleTypes...> > {
    typedef std::tuple<TupleTypes...> tuple_type;
    static constexpr int tuple_size = std::tuple_size<tuple_type>::value;
    static constexpr int value = IndexOf_<T, tuple_type, 0, tuple_size>::value;
};

template<typename T, typename ...Args>
struct IsAnyOf;

template<typename T, typename argument_type>
struct IsAnyOf<T, argument_type> { static constexpr bool value = std::is_same<T, argument_type>::value; };

template<typename T, typename Head, typename ...Tail>
struct IsAnyOf<T, Head, Tail...> { static constexpr bool value = std::is_same<T, Head>::value || IsAnyOf<T, Tail...>::value; };

template<typename Head, typename ...Tail>
struct HasDuplicates;

template<typename Head>
struct HasDuplicates<Head> { static constexpr bool value = false; };

template<typename Head, typename Tail>
struct HasDuplicates<Head, Tail> { static constexpr bool value = std::is_same<Head, Tail>::value; };

template<typename Head, typename Next, typename ...Tail>
struct HasDuplicates<Head, Next, Tail...> { static constexpr bool value = std::is_same<Head, Next>::value || IsAnyOf<Head, Tail...>::value || IsAnyOf<Next, Tail...>::value; };

// A "record" is a tuple where are elements are of different type.
// This enables access by type instead of index.
template<typename ...Args>
auto MakeRecord(Args && ...args) -> std::tuple<Args...> {
    static_assert(!HasDuplicates<Args...>::value, "The argument list must not contain two objects of same type.");
    return std::make_tuple(std::forward<Args>(args)...);
}

template<typename T, typename ...U>
T & Get(std::tuple<U...> & record) {
    static_assert(!HasDuplicates<U...>::value, "The argument list must not contain two objects of same type.");
    static constexpr unsigned index = IndexOf<T, std::tuple<U...>>::value;
    static_assert(index < (sizeof...(U)), "Out of bounds");
    return std::get< index >(record);
}

template<typename T, typename ...U>
const T & Get(const std::tuple<U...> & record) {
    static_assert(!HasDuplicates<U...>::value, "The argument list must not contain two objects of same type.");
    static constexpr unsigned index = IndexOf<T, std::tuple<U...>>::value;
    static_assert(index < (sizeof...(U)), "Out of bounds");
    return std::get< index >(record);
}

template<typename T, typename Tag>
struct StrongTypedef {
    template<typename ...Args>
    explicit StrongTypedef(Args&& ...args) : t(std::forward<Args>(args)...) { }

    explicit operator const T & () const { return t; }
    explicit operator T & () { return t; }

    const T & get() const { return t; }
    T & get() { return t; }

    friend std::ostream & operator<<(std::ostream & os, const StrongTypedef<T, Tag> & obj) { return os << obj.t; }

    T t;
};

template<typename ...Args>
struct Handy : std::tuple<Args...>
{
    template<typename ...A>
    Handy(A&& ...a) : std::tuple<Args...>(std::forward<A>(a)...) {}

    template<typename El>
    El & get() { return Get<El>(*this); }

    template<typename El>
    const El & get() const { return Get<El>(*this); }

    template<typename T>
    const T & get(T) const { return get<T>(); }

    template<typename T>
    T & get(T) { return get<T>(); }
};


using ServerId = StrongTypedef<std::string, struct ServerId_>;
using InterfaceId = StrongTypedef<std::string, struct InterfaceId_>;
using PortId = StrongTypedef<std::string, struct PortId_>;
using PortName = StrongTypedef<std::string, struct PortName_>;


#include <vector>

typedef Handy<PortId, PortName> Port;
typedef std::vector<Port> Ports;

typedef Handy<InterfaceId, Ports> Interface;
typedef std::vector<Interface> Interfaces;

typedef Handy<ServerId, Interfaces> Server;
typedef std::vector<Server> Servers;

int main()
{
    Server server {
        ServerId("s1"),
        Interfaces {
            {
                InterfaceId("i1"),
                Ports {
                    { PortId("p11"), PortName("Jolly") },
                    { PortId("p12"), PortName("Billy") }
                }
            },
            {
                InterfaceId("i2"),
                Ports {
                    { PortId("p21"), PortName("Bimbo") },
                    { PortId("p22"), PortName("Luffy") }
                }
            }
        }
    };

    auto interfaces = server.get<Interfaces>();
    std::cout << "InterfaceId: " << interfaces.at(0).get<InterfaceId>() << std::endl;

    auto ports = interfaces.at(0).get<Ports>();
    std::cout << "Port name: " << ports.at(0).get<PortName>() << std::endl;
    Handy<ServerId, InterfaceId, PortId> handy(ServerId("s1"), InterfaceId("i1"), PortId("p1"));
    handy.get<ServerId>() = ServerId{"s2"};
    handy.get<PortId>() = PortId {"p2"};

    auto record = MakeRecord(ServerId("s1"), PortId("p2"));
    std::cout << Get<ServerId>(record) << std::endl;

    std::cout << Get<ServerId>(handy) << std::endl;
    std::cout << handy.get<PortId>() << std::endl;

    std::cout << handy.get(PortId {}) << std::endl;

    handy.get(PortId()) = PortId("Port2");
    std::cout << handy.get(PortId {}) << std::endl;
}



















