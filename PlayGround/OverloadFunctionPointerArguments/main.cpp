#include <iostream>

void GetRandom(int & i) { i = 1; }
void GetRandom(double & d) { d = 3.4; }
void GetRandom(std::string & s) { s = "test"; }

template<class T> struct TypeWrapper            { typedef T Type; };
template<class T> struct TypeWrapper<const T &> { typedef T Type; };

template<class T, class U>
void TestMethod(T * inObject, void (T::*inMethod)(U))
{
    typename TypeWrapper<U>::Type arg;
    GetRandom(arg);
    (inObject->*inMethod)(arg);
}

struct Server
{
    void setPort(int) {}
    void setIP(const std::string &) {}
    void setBlah(double d) {}
};

int main()
{
    Server server;
    TestMethod(&server, &Server::setPort);
    TestMethod(&server, &Server::setIP);
    TestMethod(&server, &Server::setBlah);
    return 0;
}

