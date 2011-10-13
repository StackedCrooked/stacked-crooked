#include <iostream>


namespace Private {
namespace API {


int Sum(int a, int b)
{
}


} } // namespace API


namespace  {


int Sum(int a, int b)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return ::CoreAPI::Sum(a, b);
}


}


namespace API = Wrapper;


int main()
{    
    API::Sum(1, 2);
}
