#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>


template<typename T>
T Increment(T value, typename boost::enable_if<boost::is_arithmetic<T>, T>::type = 0)
{
	return value + 1;
}


int main()
{
	Increment(1); // ok
	Increment(1.2); // ok
	Increment('a');; // ok

	//Increment("abc"); // compiler error: not defined
}
