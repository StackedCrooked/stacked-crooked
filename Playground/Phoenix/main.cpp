#include <boost/phoenix.hpp>
#include <boost/phoenix/stl/algorithm.hpp>
#include <cassert>
#include <iostream>


struct sqrt_impl
{
    template<typename Arg>
    struct result { typedef double type; };

    template<typename Arg>
    typename result<Arg>::type operator()(Arg n)
    {
        return std::sqrt(static_cast<typename result<Arg>::type>(n));
    }
};


int main()
{
    boost::phoenix::expression::argument<1>::type a;
    boost::phoenix::expression::argument<2>::type b;
    boost::phoenix::expression::argument<3>::type c;


    // discriminant = b^2 - 4ac
    auto D = b * b - 4 * a * c;
    std::cout << "D(1, -1, -2): " << D(1.0, -1.0, -2.0) << std::endl;

    // square root
    boost::phoenix::function<sqrt_impl> sq;

    // first
    auto x1 = (-b + sq(D)) / (2 * a);
    auto x2 = (-b - sq(D)) / (2 * a);

    auto solve = [&x1, &x2](double a, double b, double c) -> std::pair<double, double> { return std::make_pair(x1(a, b, c), x2(a, b, c)); };
    auto result = solve(1.0, -1.0, -2.0);
    std::cout << "x1: " << result.first << ", x2: " << result.second << std::endl;
}
