#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include <boost/bind.hpp>


template<class ValueType, class Predicate>
struct Condition
{
	typedef ValueType argument_type; 

	Condition(const Predicate & inPredicate) :
		mPredicate(inPredicate)
	{
	}

	bool operator()(argument_type arg) const
	{
		return mPredicate(arg);
	}

	Predicate mPredicate;
};


template<class ForwardIterator, class OutputIterator, class Predicate>
void copy_if(ForwardIterator begin, ForwardIterator end, OutputIterator out, Predicate inPredicate)
{
	std::remove_copy_if(begin, end, out, std::not1(Condition<typename ForwardIterator::value_type, Predicate>(inPredicate)));
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


bool IsOdd(int value)
{
	return (value % 2) == 1;
}

int main()
{
	std::vector<int> numbers;
	numbers.push_back(0);
	numbers.push_back(1);
	numbers.push_back(2);
	numbers.push_back(3);
	numbers.push_back(4);

	std::vector<int> copy;
	copy_if(numbers.begin(), numbers.end(), std::back_inserter(copy), &IsOdd);
	Print(copy);

	return 0;
}
