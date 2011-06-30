#include <iostream>

void GetRandom(int & i) { i = 1; }
void GetRandom(double & d) { d = 3.4; }
void GetRandom(std::string & s) { s = "test"; }

template<typename T>
T CreateRandom();

template<>
int CreateRandom() { return 1; }

template<>
std::string CreateRandom() { return "Random String"; }

template<>
double CreateRandom() { return 4.5; }

template<class T> struct TypeWrapper            { typedef T Type; };
template<class T> struct TypeWrapper<const T &> { typedef T Type; };

template<class T, class DArg>
void TestSetter(T * inObject, void (T::*inMethod)(DArg))
{
    typedef typename TypeWrapper<DArg>::Type Arg;
    (inObject->*inMethod)(CreateRandom<Arg>());
}

template<class T, class Ret>
void TestGetter(const T * inObject, Ret (T::*inMethod)() const)
{
    Ret ret = (inObject->*inMethod)();
    std::cout << "Object: \"" << typeid(T).name() << "\", Method: \"" << typeid(inMethod).name() << "\", Ret: \"" << typeid(Ret).name() << "\"" << std::endl;
    std::cout << "Result: " << ret << std::endl << std::endl;
}


struct Server
{
    void setPort(int inPort)
    {
        mPort = inPort;
    }
    
    int getPort() const
    {
        return mPort;
    }

    void setIP(const std::string & inIP)
    {
        mIP = inIP;
    }
    
    const std::string & getIP() const
    {
        return mIP;
    }

    void setBlah(double inBlah)
    {
        mBlah = inBlah;
    }

    double getBlah() const
    {
        return mBlah;
    }


    std::string mIP;
    int mPort;
    double mBlah;

};

int main()
{
    Server server;
    TestSetter(&server, &Server::setPort);
    TestSetter(&server, &Server::setIP);
    TestSetter(&server, &Server::setBlah);

    TestGetter(&server, &Server::getPort);
    TestGetter(&server, &Server::getIP);
    TestGetter(&server, &Server::getBlah);
    return 0;
}

