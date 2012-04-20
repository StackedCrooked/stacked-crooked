#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits.hpp>


template<typename Function>
unsigned GetArgCount(Function *) {
    return boost::function_traits<Function>::arity;
}

template<unsigned n>
struct ArgCount
{
    enum {
        count = n
    };
};


template<typename Function>
typename boost::function_traits<Function>::result_type
CallFunction(Function * f, const std::vector<std::string> & , const ArgCount<0>&)
{
    return f();
}


template<typename Function>
typename boost::function_traits<Function>::result_type
CallFunction(Function * f, const std::vector<std::string> & args, const ArgCount<1>&)
{
    typedef typename boost::function_traits<Function>::arg1_type A1;
    return f(boost::lexical_cast<A1>(args.at(0)));
}


template<typename Function>
typename boost::function_traits<Function>::result_type
CallFunction(Function * f, const std::vector<std::string> & args, const ArgCount<2>&)
{
    typedef typename boost::function_traits<Function>::arg1_type A1;
    typedef typename boost::function_traits<Function>::arg2_type A2;
    return f(boost::lexical_cast<A1>(args.at(0)),
             boost::lexical_cast<A2>(args.at(1)));
}


template<typename Function>
typename boost::function_traits<Function>::result_type
CallFunction(Function * f, const std::vector<std::string> & args, const ArgCount<3>&)
{
    typedef typename boost::function_traits<Function>::arg1_type A1;
    typedef typename boost::function_traits<Function>::arg2_type A2;
    typedef typename boost::function_traits<Function>::arg2_type A3;
    return f(boost::lexical_cast<A1>(args.at(0)),
             boost::lexical_cast<A2>(args.at(1)),
             boost::lexical_cast<A2>(args.at(2)));
}


template<typename F, unsigned n>
typename boost::function_traits<F>::result_type CallFunction(F * f, const std::vector<std::string> & args)
{
    return CallFunction(f, args, ArgCount<boost::function_traits<F>::arity>());
}


template<typename Function>
typename boost::function_traits<Function>::result_type CallFunction(Function * f, const std::vector<std::string> & args)
{
    return CallFunction<Function, boost::function_traits<Function>::arity>(f, args);
}


// EXPORT function to a string-based API for scripting languages.
#define EXPORT(NAME) \
    std::string NAME(const std::vector<std::string> & args) { \
        if (args.size() != GetArgCount(&API::NAME)) { throw std::invalid_argument("Invalid number of arguments."); } \
        return boost::lexical_cast<std::string>(CallFunction(&API::NAME, args)); \
    }


namespace API {

int sum(int a, int b) { return a + b; }

}


// Tcl bindings for our API
namespace Tcl {

EXPORT(sum)

}


void test()
{
    std::vector<std::string> args;
    args.push_back("1");
    args.push_back("2");
    std::cout << "Sum: " << Tcl::sum(args) << std::endl;

    args.push_back("3");
    std::cout << "Too many args: " << Tcl::sum(args) << std::endl;
}


int main()
{
    try
    {
        test();
    }
    catch (const std::exception & exc)
    {
        std::cout << "Error: " << exc.what() << std::endl;
    }
}
