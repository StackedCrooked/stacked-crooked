#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>


#define TRACE std::cout << __PRETTY_FUNCTION__ << std::endl;


void test_auto()
{
	std::vector<std::string> words;
	words.push_back("one");
	words.push_back("two");
	words.push_back("three");
	words.push_back("four");
	words.push_back("five");
	words.push_back("six");
	words.push_back("seven");


	std::cout << "Using 'auto it' in for loop..." << std::endl;
	for (auto it = words.begin(); it != words.end(); ++it)
	{
		const std::string & word(*it);
		std::cout << word << " ";
	}
    std::cout << std::endl;
    std::cout << "Done.\n\n";


	std::cout << "Using for_each + lambda..." << std::endl;
	std::for_each(words.begin(), words.end(), [] (const std::string & word) { std::cout << word << " "; });
    std::cout << std::endl;
    std::cout << "Done.\n\n";


	std::cout << "Using find_if + lambda..." << std::endl;
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
    std::cout << "Done.\n\n";


	std::cout << "Using sort + lambda..." << std::endl;
	std::sort(
		words.begin(),
		words.end(),
		[] (const std::string & lhs, const std::string & rhs) {
			return lhs.size() < rhs.size();
		} );
    std::cout << "Done.\n\n";


	std::cout << "Using for_each + lambda..." << std::endl;
	std::for_each(
		words.begin(),
		words.end(),
		[] (const std::string & s) {
			std::cout << s << " ";
		} );
    std::cout << std::endl;
    std::cout << "Done.\n\n";
}


// Swap implemented with move-semantics
template<class T>
void my_swap(T & lhs, T & rhs)
{
    T tmp(std::move(lhs));
    lhs = std::move(rhs);
    rhs = std::move(tmp);
}


class Array
{
public:
    // Constructor taking length
    Array(const std::string & inName, int inLength, int inInitialValue = 0) :
        mName(inName),
        mData(malloc(inLength * sizeof(int))),
        mLength(inLength)
    {
    }


    // Destructor
    ~Array()
    {
        free(mData);
    }

    // Move constructor
    Array(Array && rhs) :
        mName("Copy of " + rhs.mName),
        mData(rhs.mData),
        mLength(rhs.mLength)
    {
        // Invalidate the right-hand side
        rhs.mData = 0;
        rhs.mLength = 0;
    }

    // Assignment operator
    Array& operator=(Array && rhs)
    {
        std::swap(mName, rhs.mName);
        std::swap(mData, rhs.mData);
        std::swap(mLength, rhs.mLength);
        return *this;
    }

    int length() const
    {
        return mLength;
    }

    const int & operator[](size_t inIndex) const 
    {
        return static_cast<int*>(mData)[inIndex];
    }

    int & operator[](size_t inIndex)
    {
        return static_cast<int*>(mData)[inIndex];
    }

private:
    Array(const Array&);
    std::string mName;
    void * mData;
    int mLength;
};


void test_move_semantics()
{
    std::cout << "Construct vector with array literal.\n";
    std::vector<double> v = { 1, 2, 3, 4 };
    std::cout << "Done.\n\n";

    std::cout << "Swap with std::move.\n";
    std::vector<double> w;
    my_swap(v, w);
    assert(v.empty());
    assert(w.size() == 4);
    std::cout << "Done.\n\n";

    std::cout << "Simplest rvalue-reference sample\n";
    class A {};
    A && a = A(); 
    std::cout << "Done.\n\n";

    // Simple Array test
    {
        std::cout << "Testing Array Normal Construction\n";
        Array array0("array", 10, 0);
        for (size_t idx = 0; idx < array0.length(); ++idx)
        {
            array0[idx] = idx;
        }

        for (size_t idx = 0; idx < array0.length(); ++idx)
        {
            if (idx != 0)
            {
                std::cout << ", ";
            }
            std::cout << array0[idx];
        }
        std::cout << "\nDone.\n\n";
    }

    // Test move constructor
    {
        std::cout << "Testing Array move constructor\n";
        Array array3(Array("anonymous", 10, 2));
        std::cout << "Done.\n\n";
    }
}


int main()
{
    test_auto();
    test_move_semantics();
	return 0;
}


