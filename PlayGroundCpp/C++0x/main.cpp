#include <algorithm>
#include <iostream>
#include <vector>
#include <string>


int main()
{
	std::vector<std::string> words;
	words.push_back("one");
	words.push_back("two");
	words.push_back("three");
	words.push_back("four");
	words.push_back("five");
	words.push_back("six");
	words.push_back("seven");


	std::cout << "Using 'auto it' in for loop" << std::endl;
	for (auto it = words.begin(); it != words.end(); ++it)
	{
		const std::string & word(*it);
		std::cout << word << std::endl;
	}

	std::cout << "\nUsing for_each + lambda" << std::endl;
	std::for_each(words.begin(), words.end(), [] (const std::string & word) { std::cout << word << std::endl; });

	std::cout << "\nUsing find_if + lambda" << std::endl;
	auto it = std::find_if(
		words.begin(),
		words.end(),
		[] (const std::string & word) {
			return word.size() == 5;
		} );

	if (it != words.end())
	{
		std::cout << "find_if succeeded: " << *it << std::endl;
	}
	else
	{
		std::cout << "find_if failed (not found)" << std::endl;
	}


	std::cout << "\nUsing sort + lambda" << std::endl;
	std::sort(
		words.begin(),
		words.end(),
		[] (const std::string & lhs, const std::string & rhs) {
			return lhs.size() < rhs.size();
		} );

	std::cout << "\nUsing for_each + lambda" << std::endl;
	std::for_each(
		words.begin(),
		words.end(),
		[] (const std::string & s) {
			std::cout << s << std::endl;
		} );

	return 0;
}

