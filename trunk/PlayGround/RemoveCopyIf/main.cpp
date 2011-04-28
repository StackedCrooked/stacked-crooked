#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

// Overload that takes a function pointer
template<class ForwardIterator, class OutputIterator, class ArgType>
void copy_if(ForwardIterator begin, ForwardIterator end, OutputIterator out, bool (*inPredicate)(ArgType))
{
    typedef std::pointer_to_unary_function<ArgType, bool> Adapter;
    std::remove_copy_if(begin, end, out, std::unary_negate<Adapter>(Adapter(inPredicate)));
}

// Overload that takes a function object
template<class ForwardIterator, class OutputIterator, class Functor>
void copy_if(ForwardIterator begin, ForwardIterator end, OutputIterator out, Functor inFunctor)
{
    std::remove_copy_if(begin, end, out, std::unary_negate<Functor>(inFunctor));
}

bool is_odd(int inValue)
{
    return inValue % 2 == 1;
}

bool is_odd_const_ref(const int & inValue)
{
    return inValue % 2 == 1;
}

struct is_odd_functor : public std::unary_function<int, bool>
{
    bool operator() (const int & inValue) const { return inValue % 2 == 1; }
};

int main()
{
	std::vector<int> numbers;
	numbers.push_back(0);
	numbers.push_back(1);
	numbers.push_back(2);
	numbers.push_back(3);

	std::vector<int> copy;

    // Functor: Ok
	copy_if(numbers.begin(), numbers.end(), std::back_inserter(copy), is_odd_functor());

    // Function pointer: Ok
	copy_if(numbers.begin(), numbers.end(), std::back_inserter(copy), is_odd);

    // Function pointer that takes const ref: Compiler error
	copy_if(numbers.begin(), numbers.end(), std::back_inserter(copy), is_odd_const_ref);
	return 0;
}

