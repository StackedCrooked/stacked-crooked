#include <iostream>
#include <vector>
#include <boost/bind.hpp>
#include <boost/function.hpp>


template<typename T>
void test(const std::vector<T> & inVector,
          const boost::function<void(const T &)> & inCallback)
{
    inCallback(inVector[0]);
}


template<typename T>
void print(const T & t)
{
    std::cout << t;
}


int main()
{
    std::vector<int> vec;
    test(vec, boost::bind(&print, _1));
    return 0;
}
