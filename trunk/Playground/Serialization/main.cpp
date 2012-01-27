#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/version.hpp>
#include <boost/tuple/tuple.hpp>
#include <sstream>
#include <iostream>


template<typename T>
std::string save(const T & value)
{
    std::ostringstream oss;
    boost::archive::text_oarchive oa(oss);
    oa << value;
    return oss.str();
}


template<typename T>
T load(const std::string & buffer)
{
    std::istringstream iss(buffer);
    boost::archive::text_iarchive ia(iss);
    T ret;
    ia >> ret;
    return ret;
}


struct MyClass
{
    MyClass() {}
    MyClass(const std::string & name) : name(name) {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & name;
    }

    std::string name;
};


int main()
{
    std::cout << "Boost version: " << BOOST_VERSION << std::endl;
    MyClass myClass("Test");
    std::string output = save(myClass);
    std::cout << "Serialized: " << output << std::endl;
    MyClass deserialized = load<MyClass>(output);
    std::cout << "Name after deserialization: " << deserialized.name << std::endl;
}
