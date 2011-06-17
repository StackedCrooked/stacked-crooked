#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <vector>

class MetaData
{
public:
    template<typename T>
    static std::string ToString(T inValue)
    {
        return boost::lexical_cast<std::string>(inValue);
    }

    template<typename T>
    static std::string ToString(const std::vector<T> & inValues)
    {
        std::stringstream ss;
        for (typename std::vector<T>::size_type idx = 0; idx < inValues.size(); ++idx)
        {
            if (idx != 0)
            {
                ss << ", ";
            }
            ss << inValues[idx];
        }
        return ss.str();
    }

    int getInt() { return 42; }

    std::vector<int> getIntVector()
    {
        std::vector<int> result;
        result.push_back(1);
        result.push_back(2);
        result.push_back(3);
        return result;
    }

    void test();
};

void MetaData::test()
{
    boost::function< std::string() > foo;

    // Ok
    foo = boost::bind(&ToString<int>, boost::bind(&MetaData::getInt, this));
    std::cout << foo() << std::endl;

    // Compiler errors
    foo = boost::bind(&ToString<int>, boost::bind(&MetaData::getIntVector, this));
    std::cout << foo() << std::endl;

    // Ok
    std::cout << ToString<int>(getIntVector()) << std::endl;
}

int main()
{
    MetaData metaData;
    metaData.test();
}

