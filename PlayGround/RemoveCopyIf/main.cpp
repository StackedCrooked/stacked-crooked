#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include <boost/bind.hpp>

template<class ForwardIterator, class OutputIterator>
void copy_if(ForwardIterator begin, ForwardIterator end, OutputIterator out, bool (*inPredicate)(typename ForwardIterator::value_type))
{
    typedef typename ForwardIterator::value_type ArgType;
    typedef std::pointer_to_unary_function<ArgType, bool> Wrapper;

    std::remove_copy_if(begin, end, out, std::unary_negate<Wrapper>(Wrapper(inPredicate)));
}

template<class ForwardIterator, class OutputIterator, class Functor>
void copy_if(ForwardIterator begin, ForwardIterator end, OutputIterator out, Functor inFunctor)
{
    std::remove_copy_if(begin, end, out, std::unary_negate<Functor>(inFunctor));
}

template<class Container>
void Print(const Container & inContainer)
{
	typename Container::const_iterator it = inContainer.begin(), end = inContainer.end();
	for (; it != end; ++it)
	{
		if (it != inContainer.begin())
		{
			std::cout << " ";
		}
		std::cout << *it;
	}
	std::cout << std::endl << std::flush;
}


bool IsOdd(int inValue)
{
    return inValue % 2 == 1;
}


struct IsOddFunctor : public std::unary_function<int, bool>
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
	numbers.push_back(4);
	numbers.push_back(5);

	std::vector<int> copy;
	copy_if(numbers.begin(), numbers.end(), std::back_inserter(copy), IsOdd);
	Print(copy);

    copy.clear();
	copy_if(numbers.begin(), numbers.end(), std::back_inserter(copy), IsOddFunctor());
	Print(copy);

	return 0;
}
