#include <stdexcept>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>


//
// C++ API
//
namespace API {

int sum(int a, int b) { return a + b; }

int multiply(int a, int b) { return a * b; }

}


//
// TCL Bindings
//
namespace Tcl {


typedef std::string Arg;
typedef std::vector<std::string> Args;


std::string sum(const Args & args)
{
    if (args.size() != 2)
    {
        throw std::invalid_argument("argc");
    }

    int result = API::sum(boost::lexical_cast<int>(args[0]),
                          boost::lexical_cast<int>(args[1]));

    return boost::lexical_cast<std::string>(result);
}


std::string multiply(const Args & args)
{
    if (args.size() != 2)
    {
        throw std::invalid_argument("argc");
    }

    int result = API::multiply(boost::lexical_cast<int>(args[0]),
                               boost::lexical_cast<int>(args[1]));

    return boost::lexical_cast<std::string>(result);
}


} // namespace Tcl


int main()
{
}
