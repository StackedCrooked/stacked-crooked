#include <boost/phoenix.hpp>
#include <boost/phoenix/stl/algorithm.hpp>
#include <iostream>


double my_sqrt(double d)
{
    return std::sqrt(d);
}


int main()
{
    boost::phoenix::expression::argument<1>::type a;
    boost::phoenix::expression::argument<2>::type b;
    boost::phoenix::expression::argument<3>::type c;


    // discriminant = b^2 - 4ac
    auto D = b * b - 4 * a * c;
    std::cout << "D(1, -1, -2): " << D(1.0, -1.0, -2.0) << std::endl;

    // square root
    auto sq = boost::phoenix::bind(my_sqrt, a);
    std::cout << "sq: " << sq(D(1.0, -1.0, -2.0)) << std::endl;

    // sq ○ D
    auto sqD = sq(D);
    std::cout << "sq(D): " << sqD(1.0, -1.0, -2.0) << std::endl; // error: 'sqD' cannot be used as a function

    //auto x1 = (-b + sq(D)) / (2 * a);
    //auto x2 = (-b - sq(D)) / (2 * a);
}
