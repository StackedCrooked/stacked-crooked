#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <windows.h>
#include <assert.h>


// Concatenates a value and a vector
template<class T>
void concat(const T & inFirst,
			const std::vector<T> & inItems,
			std::vector<T> & outItems)
{
	outItems.push_back(inFirst);
	std::copy(inItems.begin(), inItems.end(), std::back_inserter(outItems));
			
}


template<class T>
void getPermutations(const std::vector<T> & inItems,
	   				 std::vector<std::vector<T> > & outPermutations)
{
	if (inItems.size() <= 1)
	{
		outPermutations.push_back(inItems);
	}
	else if (inItems.size() == 2)
	{
		outPermutations.push_back(inItems);

		std::vector<T> swapped = inItems;
		std::swap(swapped[0], swapped[1]);
		outPermutations.push_back(swapped);
	}
	else
	{
		for (int masterIdx = 0; masterIdx != inItems.size(); ++masterIdx)
		{
			// Select a master element
			const T & master = inItems[masterIdx];

			// Select the subjects
			std::vector<T> subjects;
			for (int subjectIdx = 0; subjectIdx != inItems.size(); ++subjectIdx)
			{
				if (subjectIdx != masterIdx)
				{
					subjects.push_back(inItems[subjectIdx]);
				}
			}
			
			std::vector<std::vector<T> > permutedSubjects;
			getPermutations(subjects, permutedSubjects);
			for (int i = 0; i != permutedSubjects.size(); ++i)
			{
				std::vector<T> permutation;
				concat(master, permutedSubjects[i], permutation);
				outPermutations.push_back(permutation);
			}
		}
	}
}


void getIntPermutations(int n, std::vector<std::vector<int> > & outPermutations)
{
	std::vector<int> items;
	for (int i = 1; i <= n; ++i)
	{
		items.push_back(i);
	}
	getPermutations(items, outPermutations);
}


void printPermutations(int n, std::ostream & out)
{
	std::vector<std::vector<int> > permutations;
	getIntPermutations(n, permutations);
	for (size_t i = 0; i != permutations.size(); ++i)
	{
		std::vector<int> p = permutations[i];
		for (size_t j = 0; j != p.size(); ++j)
		{
			out << p[j];
		}
		out << std::endl;
	}
}


int main()
{
	std::cout << "Give number: " << std::endl;
	int n;
	std::cin >> n;

	if (n <= 0 || n > 100)
	{
		std::cout << "Number must be positive and smaller than 100" << std::endl;
	}

	std::cout << "Calculating..." << std::endl;
	printPermutations(n, std::cout);

	MessageBox(0, L"Program is done", L"Permutations", MB_OK);
	return 0;
}
