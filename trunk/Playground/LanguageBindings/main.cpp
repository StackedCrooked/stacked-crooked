#include <stdexcept>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>


struct Calculator
{
    int sum(int a, int b) { return a + b; }

    int multiply(int a, int b) { return a * b; }
};


// Bindings to Tcl
struct TclCalculator
{
    typedef std::string Arg;
    typedef std::vector<Arg> Args;

    std::string sum(const Args & args)
    {
        if (args.size() != 2)
        {
            throw std::invalid_argument("argc");
        }

        int result = c.sum(boost::lexical_cast<int>(args[0]),
                         boost::lexical_cast<int>(args[1]));

        return boost::lexical_cast<std::string>(result);
    }

    std::string multiply(const Args & args)
    {
        if (args.size() != 2)
        {
            throw std::invalid_argument("argc");
        }

        int result = c.multiply(boost::lexical_cast<int>(args[0]),
                              boost::lexical_cast<int>(args[1]));

        return boost::lexical_cast<std::string>(result);
    }


    Calculator c;
};


int main()
{
}
